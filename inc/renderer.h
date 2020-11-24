#ifndef RENDERER_H
#define RENDERER_H

#include <unordered_map>

#include "object.h"

class Renderer {
	public:
		Renderer(const int &width, const int &height, const float &fov);
		~Renderer();
		
		void updateFOV(const float &fov);
		void updateRenderingPlane();

		void render(const vec3f &camera_position, const vec3f &camera_rotation, const std::vector<Object*> &objects);

		u32 * getScreenBuffer() const;

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
