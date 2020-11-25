#ifndef RENDERER_CPU_H
#define RENDERER_CPU_H

#include "graphics/renderer.h"
#include "object.h"

class Renderer_CPU : public Renderer {
	public:
		Renderer_CPU(const int &width, const int &height, const float &fov) : Renderer(width, height, fov) {}
		void render(const vec3f &camera_position, const vec3f &camera_rotation, const std::vector<Object*> &objects);
};

#endif
