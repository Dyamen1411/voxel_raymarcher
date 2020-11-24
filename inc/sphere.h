#include "object.h"

class Sphere : public Object {
	public:
		Sphere(const float &radius, const vec3f &pos);

		void myUpdate(const long int &time);
		float SDF(const vec3f &pos) const;
	
	private:
		float m_radius;
		vec3f m_pos;
};
