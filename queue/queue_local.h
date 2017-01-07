#pragma once

#include "queue_base.h"

#include "common/experimental_cxx_features.h"
#include "result_collector.h"

#include <queue>

struct QueueLocal : Queue {
	std::queue<Job> storage;
	ResultCollector result_collector;

	QueueLocal(std::string_view jobspec, std::string_view outfile) {
		Job root_job(jobspec.data());
		logging::console->info("Root job: {}", root_job);
		auto&& jobs = root_job.split({100, 100});
		for (auto&& job : jobs) {
			logging::console->info("{}", job);
			storage.push(job);
		}

		result_collector.set_output(outfile.data());
		result_collector.set_size(root_job.viewport_size_px);
	}

	void push(Job job) {
		storage.push(std::move(job));
	}

	void pop() {
		storage.pop();
	}

	Job front() {
		logging::console->trace("{} jobs remaining", storage.size());
		return storage.front();
	}

	bool empty() {
		return storage.empty();
	}

	void submit_result(JobResult&& result) {
		result_collector.collect(std::move(result));
	}

	void finalize() {
		result_collector.render();
	}
};