#ifndef SPHERE_H
#define SPHERE_H

#include "object.h"

class Sphere : public Object {
	public:
		Sphere(const float &radius, const vec3f &pos);

		void myUpdate(const long int &time);
		float SDF(const vec3f &pos) const;
		
		static Sphere * getIdentifier() {
			static Sphere * identifier = new Sphere(0, { 0, 0, 0});
			return identifier;
		}
	
	private:
		float m_radius;
		vec3f m_pos;
};

#endif
