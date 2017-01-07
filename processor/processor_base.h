#pragma once

#include "queue/queue_base.h"

struct Processor {
	Queue *queue;

	Processor(Queue *queue) : queue(queue) {
	}

	virtual void process(Job& job) = 0;

	virtual void process_queue() {
		while (!queue->empty()) {
			Job&& job = queue->front();
			process(job);
			queue->pop();
		}
	}

	virtual void finalize() {}

	virtual ~Processor() {
		logging::console->debug("Deleting queue");
		delete queue;
	}
};