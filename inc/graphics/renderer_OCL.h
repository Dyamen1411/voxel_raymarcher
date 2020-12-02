#ifndef RENDERER_OCL_H
#define RENDERER_OCL_H

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#include <string>

#include "graphics/renderer.h"
#include "object.h"

class Renderer_OCL : public Renderer {
	public:
		Renderer_OCL(const int &width, const int &height, const float &fov);
		~Renderer_OCL();
		int initOCL();
		void render(const vec3f &camera_position, const vec3f &camera_rotation, const std::vector<Object*> &objects);
		int compileSources(const std::string &path);
		void makeKernel();
		void updateDy(const float &theta);

	private:
		// opencl variables
		cl::Platform m_cl_platform;
		std::vector<cl::Device> m_cl_devices;
		cl::Context m_cl_context;
		cl::CommandQueue m_cl_queue;
		cl::Kernel m_cl_kernel;
		cl::Program m_cl_program;
};

#endif
