#pragma once

#include <ostream>

template <typename T>
struct vec2 {
	using type = T;
	T x, y;

	friend std::ostream& operator<<(std::ostream& ost, const vec2& v) {
		return ost << '(' << v.x << ',' << v.y << ')';
	}
};

template<typename T>
vec2<T> operator+(const vec2<T> a, const vec2<T>& b) {
	return { a.x + b.x, a.y + b.y };
}

template<typename scalar, typename T>
vec2<T> operator+(const vec2<T> v, const scalar x) {
	return { v.x + x, v.y + x };
}

template<typename scalar, typename T>
vec2<T> operator-(const vec2<T> v, const scalar x) {
	return { v.x - x, v.y - x };
}

template<typename scalar, typename T>
vec2<T> operator/(const vec2<T> v, const scalar x) {
	return { v.x / x, v.y / x };
}

template<typename T1, typename T2>
vec2<std::common_type_t<T1, T2>> operator/(const vec2<T1> a, const vec2<T2> b) {
	return { a.x / b.x,a.y / b.y };
}

template<typename T1, typename T2>
vec2<std::common_type_t<T1, T2>> operator*(const vec2<T1> a, const vec2<T2> b) {
	return { a.x * b.x,a.y * b.y };
}

template<typename T1, typename T2>
vec2<std::common_type_t<T1, T2>> operator%(const vec2<T1> a, const vec2<T2> b) {
	return { a.x % b.x,a.y % b.y };
}

using vec2i = vec2<int>;
using vec2l = vec2<long long>;
using vec2f = vec2<float>;
using vec2d = vec2<double>;