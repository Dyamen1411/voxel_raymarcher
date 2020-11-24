#include <math.h>

#include "sphere.h"

Sphere::Sphere(const float &radius, const vec3f &pos) {
	m_radius = radius;
	m_pos = pos;
}

void Sphere::myUpdate(const long int &time) {

}

float Sphere::SDF(const vec3f &pos) const {
	return sqrtf(
			(pos.x - m_pos.x)*(pos.x - m_pos.x) + 
			(pos.y - m_pos.y)*(pos.y - m_pos.y) + 
			(pos.z - m_pos.z)*(pos.z - m_pos.z)
		    ) - m_radius;
}
