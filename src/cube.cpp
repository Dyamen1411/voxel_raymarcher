#include <math.h>

#include "cube.h"

Cube::Cube(const float &size, const vec3f &pos) {
	m_size = size;
	m_pos = pos;

	static int first = 1;
	if (first) {
		first = 0;
		Object::getIdentifiers()->push_back(Cube::getIdentifier());
	}
}

void Cube::myUpdate(const long int &time) {

}

float Cube::SDF(const vec3f &pos) const {
/*	vec3f q = { 
		std::max(abs(pos.x) - m_size, 0.f), 
		std::max(abs(pos.y) - m_size, 0.f),
		std::max(abs(pos.z) - m_size, 0.f)
	};
	return sqrtf(q.x*q.x + q.y*q.y + q.z*q.z);*/

	return pos.y;
}
