#ifndef DEF_H
#define DEF_H

#define IINF 0x7fffffff
#define FINF 0x7f800000

typedef unsigned char u8;
typedef char16_t u16;
typedef char32_t u32;

template<class T>
struct vec3 { T x, y, z; };
typedef vec3<float> vec3f;

template<class T>
vec3<T> normalize(const vec3<T> &v) {
	T len = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
	if (len != 0) {
		return { v.x / len, v.y / len, v.z / len };
	} else {
		return v;
	}
}

template<class T>
T dot(const vec3<T> &a, const vec3<T> &b) {
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

template<class T>
vec3<T> cross(const vec3<T> &a, const vec3<T> &b) {
	return {
		a.y*b.z - a.z*b.y,
		a.z*b.x - a.x*b.z,
		a.x*b.y - a.y*b.x
		};
}

template<class T>
vec3f reflect(const vec3<T> &I, const vec3<T> &N) {
	const float c = 2.0 * dot(N, I);
	return { I.x - c*N.x, I.y - c*N.y, I.z - c*N.z };
}

template<class T>
struct mat3 {
	vec3<T> a, b, c;
};
typedef mat3<float> mat3f;

template<class T>
vec3<T> mul(const mat3<T> &m, const vec3<T> &v) {
	return {
		m.a.x*v.x + m.a.y*v.y + m.a.z*v.z,
		m.b.x*v.x + m.b.y*v.y + m.b.z*v.z,
		m.c.x*v.x + m.c.y*v.y + m.c.z*v.z
	};
};

template<class T>
T clamp(const T &v, const T &min, const T &max) {
	if (v > max) {
		return max;
	}

	if (v < min) {
		return min;
	}

	return v;
}

#include "uuid.h"

#endif
