#ifndef CUBE_H
#define CUBE_H

#include "object.h"

class Cube : public Object {
	public:
		Cube(const float &size, const vec3f &pos);

		void myUpdate(const long int &time);
		float SDF(const vec3f &pos) const;

		static Cube * identifier;

		static Cube * getIdentifier() {
			static Cube * identifier = new Cube(0, { 0, 0, 0 });
			return identifier;
		}

	private:
		float m_size;
		vec3f m_pos;
};

#endif
