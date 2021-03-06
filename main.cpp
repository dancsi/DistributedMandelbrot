#include <chrono>
#include <cstdlib>
#include <iostream>
#include <mpi.h>
#include <deque>

#include "flags.h"

#include "processor/mpi_server.h"
#include "processor/worker_mt.h"
#include "processor/worker_st.h"

#include "queue/queue_base.h"
#include "queue/queue_local.h"
#include "queue/queue_mpi.h"

#include "common/job.h"
#include "common/logging.h"
#include "mpi/message_tags.h"
#include "mpi/mpi_helpers.h"

using namespace std;

logging::console_logger_t logging::console;


int main(int argc, char** argv) {
	using namespace MPI::detail;
	TypeLookup<gsl::span<char>>::value;

	using logging::console;
	logging::setup(console);

	const flags::args args(argc, argv);
	bool is_standalone = args.get<bool>("standalone", false);
	auto processor_type = args.get<string>("processor");

	if (!processor_type) {
		console->error("You must specify the processor type");
		exit(1);
	}
	
	if (*processor_type == "server") {
		is_standalone = true;
	}

	Processor *processor = nullptr;
	Queue *queue = nullptr;

	if (is_standalone) {
		string jobspec = args.get<string>("jobspec", "jobspec.json");
		string outfile = args.get<string>("outfile", "out.ppm");

		queue = new QueueLocal(jobspec, outfile);
	}
	else {
		queue = new QueueMPI();
	}

	if (*processor_type == "server") {
		processor = new MPIServer(queue);
	}
	else if (*processor_type == "worker-st") {
		processor = new WorkerST(queue);
	}
	else if (*processor_type == "worker-mt") {
		int n_threads = args.get<int>("num_threads", std::thread::hardware_concurrency());
		processor = new WorkerMT(queue, n_threads);
	}
	else {
		console->error("The processor must be one of: server, worker-st, worker-mt");
		exit(1);
	}

	auto start_time = chrono::high_resolution_clock::now();

	processor->process_queue();
	processor->finalize();

	auto duration = chrono::high_resolution_clock::now() - start_time;
	console->info("Time used for calculation: {} seconds", std::chrono::duration_cast<chrono::seconds>(duration).count());

	delete processor;

    return 0;
}