#ifndef RENDERER_CPU_H
#define RENDERER_CPU_H

#include "graphics/renderer.h"
#include "object.h"

class Renderer_CPU : public Renderer {
	public:
		Renderer_CPU(const int &width, const int &height, const float &fov) : Renderer(width, height, fov) {}
		
		void render(const vec3f &camera_position, const vec3f &camera_rotation, const std::vector<Object*> &objects);

	private:
		float m_distance_to_plane;
		float * m_rendering_plane_x;
		float * m_rendering_plane_y;

		int m_width, m_height;
		float m_aspect_ratio;
		double * m_depth_buffer;
		u32 * m_screen_buffer;
};

#endif
