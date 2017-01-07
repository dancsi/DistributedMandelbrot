#pragma once

#include "gsl/gsl"

#include "common/vec.h"

struct JobResult {
	using T = float;
	T *result;

	struct Header {
		vec2i dimensions, position;
		uint32_t max_iterations;
		gsl::span<char> as_bytes() {
			return { reinterpret_cast<char*>(this), sizeof(*this) };
		}

		friend std::ostream& operator <<(std::ostream& ost, const Header& h) {
			return ost << "<Header " << h.max_iterations << " " << h.dimensions << " " << h.position << ">";
		}
	} header;

	JobResult(vec2i dimensions, vec2i position, uint32_t max_iterations) : header{ dimensions, position, max_iterations } {
		result = new T[dimensions.x*dimensions.y];
	}

	T* operator[](size_t idx) {
		return result + idx*header.dimensions.x;
	}

	gsl::span<T> as_bytes() {
		return gsl::make_span(result, header.dimensions.x*header.dimensions.y);
	}

	JobResult(Header& header) : JobResult(header.dimensions, header.position, header.max_iterations) {	}

	JobResult(Header& header, gsl::span<T> bytes) : JobResult(header) {
		std::copy(bytes.begin(), bytes.end(), result);
	}

	JobResult() : result(nullptr), header{ {0, 0}, {0, 0}, 0 } {}
};