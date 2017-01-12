#pragma once

#include <complex>

#include "common/job.h"
#include "common/job_result.h"

struct Kernel {
	uint32_t max_iterations;
	Kernel(uint32_t max_iterations) : max_iterations(max_iterations) {}

	JobResult process(Job& job) {
		JobResult ret(job.viewport_size_px, job.pos_px, max_iterations);

		for (int i = 0; i < job.viewport_size_px.y; i++) {
			for (int j = 0; j < job.viewport_size_px.x; j++) {
				vec2d pos = job.pos + vec2i{ j, i } *(job.viewport_size / job.viewport_size_px);
				ret[i][j] = calc(pos);
			}
		}

		return ret;
	}

	float calc(vec2d pos) {
		std::complex<double> z = 0;
		std::complex<double> c(pos.x, pos.y);
		uint32_t iteration_count = 0;

		while (iteration_count < max_iterations && norm(z) < 1<<16) {
			z = z*z + c;
			iteration_count++;
		}

		double ret = iteration_count;
		if (iteration_count < max_iterations) {
			double log_zn = log(norm(z)) / 2;
			double nu = log(log_zn / log(2.0)) / log(2.0);
			ret += 1 - nu;
		}

		return ret;
	}
};