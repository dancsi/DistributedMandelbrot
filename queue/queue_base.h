#pragma once

#include "common/job.h"
#include "common/job_result.h"

struct Queue {
	virtual void push(Job job) = 0;
	virtual void pop() = 0;
	virtual Job front() = 0;
	virtual bool empty() = 0;

	virtual void submit_result(JobResult&& result) = 0;
	virtual void finalize() {}

	virtual ~Queue() {}
};