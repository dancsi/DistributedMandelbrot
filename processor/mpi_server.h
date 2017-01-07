#pragma once

#include "processor_base.h"

#include "common/logging.h"
#include "mpi/mpi_helpers.h"

struct MPIServer : Processor {

	struct ServerContext : MPI::Context {
		ServerContext(int *argc, char*** argv) : Context(argc, argv) {
			logging::console->trace("Process type is server");
			logging::console->debug("Server rank is {}", rank);
			MPI::Bcast(rank, 0, comm);
		}
	} context;

	int n_jobs_sent;

	MPIServer(Queue *queue) : context(nullptr, nullptr), Processor(queue), n_jobs_sent(0) {

	}

	void process(Job& job) {
		MPI_Status status;
		int dummy_data;
		MPI::Recv(dummy_data, MPI_ANY_SOURCE, Tag::job_request, context.comm, &status);
		int source = status.MPI_SOURCE;
		logging::console->debug("Received work request from worker {}", source);
		MPI::Send(job.as_bytes(), source, context.comm, Tag::job_reply_new_job);
		n_jobs_sent++;
	}

	void finalize() override {
		int dummy_data;
		MPI_Status status;

		for (int i = 1; i < context.world_size; i++) {
			MPI::Recv(dummy_data, MPI_ANY_SOURCE, Tag::job_request, context.comm, &status);
			int source = status.MPI_SOURCE;
			logging::console->debug("Terminated worker {}", source);
			MPI::Send(dummy_data, source, context.comm, Tag::job_reply_no_jobs);
		}

		MPI::Barrier(context.comm);
		logging::console->info("Synchronized");
		logging::console->info("Want to receive {} jobs", n_jobs_sent);

		JobResult::Header header;
		for (int i = 0; i < n_jobs_sent; i++) {
			MPI::Recv(header.as_bytes(), MPI_ANY_SOURCE, Tag::job_results_header, context.comm, &status);
			int source = status.MPI_SOURCE;
			logging::console->debug("Received {} from worker {}", header, source);

			JobResult res(header);
			MPI::Recv(res.as_bytes(), source, Tag::job_results_data, context.comm, &status);
			logging::console->info("Received job result from worker {}", source);

			queue->submit_result(std::move(res));
		}

		logging::console->info("Collected all job results");

		queue->finalize();
	}
};