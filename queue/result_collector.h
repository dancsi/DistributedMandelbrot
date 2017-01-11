#pragma once

#include "common/vec.h"

#include "color.hpp"

#include <experimental/filesystem>
#include <ios>

namespace fs = std::experimental::filesystem;

struct ResultCollector {
	vec2i size;
	fs::path path;
	const static int palette_size = 255;

	template<typename T>
	struct MatrixWrapper {
		using element_type = T;
		T* data;
		int n, m;

		void allocate(int n, int m) {
			data = new T[n*m];
		}

		MatrixWrapper() : data(nullptr), n(0), m(0) { }

		MatrixWrapper(int n, int m) : n(n), m(m) {
			allocate(n, m);
		}

		void deallocate() {
			if (data != nullptr) {
				delete data;
			}
			data = nullptr;
		}

		~MatrixWrapper() {
			deallocate();
		}

		void resize(int new_n, int new_m) {
			deallocate();
			n = new_n; m = new_m;
			allocate(n, m);
		}

		T* operator[] (size_t idx) {
			return data + idx*m;
		}

		gsl::span<T> as_span() {
			return gsl::make_span(data, n*m );
		}
	};

	MatrixWrapper<float> data;
	MatrixWrapper<color::rgb<uint8_t>> img;

	ResultCollector() {

	}

	void set_output(fs::path path) {
		ResultCollector::path = path;
	}

	void set_size(vec2i size) {
		ResultCollector::size = size;
		data.resize(size.y, size.x);
	}

	void collect(JobResult&& result) {
		int n = result.header.dimensions.y;
		int m = result.header.dimensions.x;
		int si = result.header.position.y;
		int sj = result.header.position.x;

		for (int i = 0; i < n; i++) {
			for (int j = 0; j < m; j++) {
				data[si + i][sj + j] = result[j][i];
			}
		}
	}

	void normalize() {
		auto span = data.as_span();
		auto minmax = std::minmax_element(span.begin(), span.end());
		float min_el = *minmax.first, max_el = *minmax.second;
		float range = max_el - min_el;

		img.resize(data.n, data.m);
		auto img_span = img.as_span();
		std::transform(span.begin(), span.end(), img_span.begin(), 
			[&](float fl) -> color::rgb<uint8_t>
		{
			//fl = (fl - min_el) / range;
			return color::rgb<uint8_t>(color::hsv<float>{0.95f + 20 * fl, 100*0.6f, 100*1.0f});
		});
	}

	void render() {
		logging::console->info("Rendering image");

		normalize();

		std::ofstream fout(path.string(), std::ios::binary);

		fout << "P6\n";
		fout << size.x << " " << size.y << "\n";
		fout << palette_size << "\n";

		auto img_span = img.as_span();
		fout.write(reinterpret_cast<char*>(img_span.data()), img_span.size_bytes());

		fout.close();
		logging::console->info("Rendered to {}", path.string());
	}

	~ResultCollector() {

	}
};