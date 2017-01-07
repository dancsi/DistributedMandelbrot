#pragma once

#include "processor_base.h"

#include <vector>

#include "kernel/kernel.h"

struct WorkerST : Processor {
	Kernel kernel;
	std::vector<JobResult> results;

	WorkerST(Queue *queue) : Processor(queue), kernel(1000) {}

	void process(Job& job) {
		logging::console->info("Processing {}", job);
		results.push_back(kernel.process(job));
	}

	void finalize() override {
		logging::console->info("Will submit {} results", results.size());
		for (JobResult& res : results) {
			queue->submit_result(std::move(res));
		}

		queue->finalize();
	}
};