#ifndef RENDERER_OCL_H
#define RENDERER_OCL_H

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>

#include "graphics/renderer.h"
#include "object.h"

class Renderer_OCL : public Renderer {
	public:
		Renderer_OCL(const int &width, const int &height, const float &fov);
		~Renderer_OCL();
		int initOCL();
		void render(const vec3f &camera_position, const vec3f &camera_rotation, const std::vector<Object*> &objects);

	private:
		// opencl variables
		cl::Platform m_cl_platform;
		cl::Device m_cl_device;
		cl::Context m_cl_context;
		cl::CommandQueue m_cl_queue;
		cl::Kernel m_cl_kernel;
		cl::Program m_cl_program;
};

#endif
