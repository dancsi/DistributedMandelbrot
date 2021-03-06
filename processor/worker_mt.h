#pragma once

#include "processor_base.h"

#include "concurrentqueue/concurrentqueue.h"
#include "kernel/kernel.h"

#include <thread>
#include <condition_variable>
#include <vector>

struct WorkerMT : Processor {
	Kernel kernel;
	std::vector<JobResult> results;
	int n_threads;

	WorkerMT(Queue *queue, int n_threads) : Processor(queue), kernel(1000), n_threads(n_threads) {}

	moodycamel::ConcurrentQueue<Job> queue_master_slave;
	moodycamel::ConcurrentQueue<JobResult> queue_slave_master;
	bool everything_done;
	std::atomic<int> n_idle_workers;
	std::mutex mut;
	std::condition_variable cv_worker_ready;

	void process(Job& job) {
		JobResult&& res = kernel.process(job);
		queue_slave_master.enqueue(std::move(res));
	}

	void worker_func(int id) {
		n_idle_workers++;
		int job_counter = 0;
		Job job = Job::dummy();
		while (!everything_done) {
			cv_worker_ready.notify_one();
			while (queue_master_slave.try_dequeue(job)) {
				job_counter++;
				n_idle_workers--;
				process(job);
				n_idle_workers++;
			}
			std::this_thread::yield();
		}
		logging::console->info("Worker {} shutting down, completed {} jobs", id, job_counter);
	}

	void process_queue() override {
		std::vector<std::thread> threads;

		everything_done = false;
		n_idle_workers.store(0);
		for (int i = 0; i < n_threads; i++) {
			std::thread worker(&WorkerMT::worker_func, this, i);
			threads.push_back(std::move(worker));
		}

		logging::console->info("Started {} threads", n_threads);

		int jobs_fetched = 0;
		while (!queue->empty()) {
			{
				std::unique_lock<std::mutex> lk(mut);
				cv_worker_ready.wait(lk);
			}
			int jobs_to_fetch = n_idle_workers.load();
			for (int i = 0; (i < jobs_to_fetch) && !queue->empty(); i++) {
				jobs_fetched++;
				Job&& job = queue->front(); queue->pop();
				queue_master_slave.enqueue(job);
			}
			//valjalo bi ovde obavestiti workere da mogu da rade
		}

		int jobs_transferred = 0;
		while (jobs_transferred < jobs_fetched) {
			JobResult result;
			while (queue_slave_master.try_dequeue(result)) {
				results.push_back(std::move(result));
				jobs_transferred++;
			}
			std::this_thread::yield();
		}

		everything_done = true;

		for (auto&& worker : threads) {
			worker.join();
		}
	}

	void finalize() override {
		logging::console->info("Will submit {} results", results.size());
		for (JobResult& res : results) {
			queue->submit_result(std::move(res));
		}

		queue->finalize();
	}
};