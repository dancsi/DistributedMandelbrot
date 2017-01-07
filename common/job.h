#pragma once

#include <fstream>
#include <ostream>
#include <string>
#include <vector>

#include <gsl/gsl>

#include "experimental_cxx_features.h"
#include "logging.h"
#include "vec.h"

#include "json.hpp"

using json = nlohmann::json;
namespace fs = std::experimental::filesystem;

struct Job {
	vec2d pos;
	vec2d viewport_size;
	vec2i pos_px;
	vec2i viewport_size_px;

	Job(const fs::path path) {
		if (!fs::exists(path)) {
			logging::log_and_throw("The requested file {} does not exist", path.string());
		}
		std::ifstream fin(path.string());
		json config;
		fin >> config;
		pos = { config["pos"][0], config["pos"][1] };
		viewport_size = { config["viewport_size"][0], config["viewport_size"][1] };
		viewport_size_px = { config["viewport_size_px"][0], config["viewport_size_px"][1] };
		pos_px = { 0, 0 };
	}

	Job(const vec2d pos, const vec2d viewport_size, const vec2i pos_px, const vec2i viewport_size_px) 
		: pos(pos), viewport_size(viewport_size), pos_px(pos_px), viewport_size_px(viewport_size_px) {}

	static Job dummy() {
		return Job({ 0, 0 }, { 0, 0 }, { 0, 0 }, {0, 0});
	}

	std::vector<Job> split(vec2i chunk_size) {
		vec2i n_chunks = (viewport_size_px + chunk_size - 1) / chunk_size;
		std::vector<int> pixels_x(n_chunks.x - 1, chunk_size.x), pixels_y(n_chunks.y - 1, chunk_size.y);

		vec2i last_chunk = viewport_size_px % chunk_size;
		if (last_chunk.x == 0) last_chunk.x = chunk_size.x;
		if (last_chunk.y == 0) last_chunk.y = chunk_size.y;

		pixels_x.push_back(last_chunk.x);
		pixels_y.push_back(last_chunk.y);

		vec2d pixel_ratio = viewport_size / viewport_size_px;
		std::vector<int> start_pixels_x(n_chunks.x, 0), start_pixels_y(n_chunks.y, 0);

		for (int i = 1; i < n_chunks.x; i++) {
			start_pixels_x[i] = start_pixels_x[i - 1] + pixels_x[i - 1];
		}
		for (int i = 1; i < n_chunks.y; i++) {
			start_pixels_y[i] = start_pixels_y[i - 1] + pixels_y[i - 1];
		}

		std::vector<Job> ret;
		for (int i = 0; i < n_chunks.x; i++) {
			for (int j = 0; j < n_chunks.y; j++) {
				vec2i new_size_px = { pixels_x[i], pixels_y[j] };
				vec2d new_size = new_size_px * pixel_ratio;

				vec2i new_pos_px = { start_pixels_x[i], start_pixels_y[j] };
				vec2d new_pos = pos + new_pos_px * pixel_ratio;

				ret.push_back(Job(new_pos, new_size, pos_px + new_pos_px, new_size_px));
			}
		}

		return ret;
	} 

	friend std::ostream& operator <<(std::ostream& ost, const Job& j) {
		return ost << "<Job " << j.pos << " " << j.viewport_size << " " << j.pos_px << " " << j.viewport_size_px << ">";
	}

	gsl::span<char> as_bytes() {
		int sz = sizeof(*this);
		return { reinterpret_cast<char*>(this), sz };
	}

	Job(gsl::span<char> bytes) {
		memcpy(reinterpret_cast<void*>(this), bytes.data(), bytes.size_bytes());
	}
};

