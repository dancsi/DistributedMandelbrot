#pragma once

#include "queue_base.h"

#include <optional>
#include <queue>

struct QueueMPI : Queue {
	struct WorkerContext : MPI::Context {
		int server_rank;

		WorkerContext(int *argc, char*** argv) : Context(argc, argv) {
			logging::console->trace("Process type is worker");
			MPI::Bcast(server_rank, 0, comm);
			logging::console->info("Worker {}: server rank is {}", rank, server_rank);
		}
	} context;

	const int buffer_size = 2;

	std::queue<Job> buffer;
	bool remote_queue_empty;

	std::optional<Job> pull_single() {
		char data[sizeof(Job)];
		//console->debug("Sending job request");
		int dummy;
		MPI::Send(dummy, context.server_rank, context.comm, Tag::job_request);
		MPI_Status status;
		logging::console->debug("Waiting for job reply on worker {}", context.rank);
		MPI::Recv(gsl::span<char>{data}, context.server_rank, MPI_ANY_TAG, context.comm, &status);
		int tag = status.MPI_TAG;
		Job j = Job::dummy();
		switch (tag) {
		case Tag::job_reply_new_job:
			j = Job{ gsl::span<char>(data, sizeof(Job)) };
			logging::console->info("new job: {} on worker {}", j, context.rank);
			return j;
		case Tag::job_reply_no_jobs:
			logging::console->info("No jobs left");
			remote_queue_empty = true;
			MPI::Barrier(context.comm);
			logging::console->info("Synchronized");
		}
		return {};
	}

	void fill_buffer() {
		while (!remote_queue_empty && buffer.size() < buffer_size) {
			auto&& job = pull_single();
			if (job.has_value())
				buffer.push(*job);
			else
				break;
		}
	}

	QueueMPI() : context(nullptr, nullptr), remote_queue_empty(false) {

	}

	~QueueMPI() {
		logging::console->trace("Destroying MPI queue");
	}

	bool empty() {
		fill_buffer();
		return remote_queue_empty && buffer.empty();
	}

	void push(Job job) {
		logging::log_and_throw("The MPI virtual queue does not support pushing");
	}

	void pop() {
		if (empty()) {
			logging::log_and_throw("The MPI virtual queue is empty");
		}
		buffer.pop();
		fill_buffer();
	}

	Job front() {
		if (empty()) {
			logging::log_and_throw("The MPI virtual queue is empty");
		}
		return buffer.front();
	}

	void submit_result(JobResult&& result) override {
		logging::console->info("Submitting result");
		logging::console->debug("Header: {}", result.header);
		MPI::Send(result.header.as_bytes(), context.server_rank, context.comm, Tag::job_results_header);
		MPI::Send(result.as_bytes(), context.server_rank, context.comm, Tag::job_results_data);
	}
};