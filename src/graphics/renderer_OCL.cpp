#include "graphics/renderer_OCL.h"

//#define __CL_ENABLE_EXCEPTIONS

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#define MAX_STEPS 255
#define MAX_DISTANCE 100
#define EPSILON 0.0001

Renderer_OCL::Renderer_OCL(const int &width, const int &height, const float &fov) : Renderer(width, height, fov) {
	if (initOCL()) {
		printf("Could not initialize OpenCL!\nExiting...\n");
		exit(EXIT_FAILURE);
	}
}

Renderer_OCL::~Renderer_OCL() {}

int Renderer_OCL::compileSources(const std::string &path) {

	// This is dumb.
	std::string sources;
	{
		std::stringstream str;
		std::ifstream stream;
		stream.open(path.c_str());
		str << stream.rdbuf();
		sources = str.str();
	}

	m_cl_program = cl::Program(
			m_cl_context, 
			cl::Program::Sources(1, std::make_pair(sources.c_str(), sources.length())
	    ));

	try {
	    m_cl_program.build({m_cl_devices[0]});
	} catch (const cl::Error&) {
	    std::cerr
		<< "OpenCL compilation error" << std::endl
		<< m_cl_program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(m_cl_devices[0])
		<< std::endl;
//	    exit(EXIT_FAILURE);
	    return 1;
	}

	return 0;
}

void Renderer_OCL::makeKernel() {
	m_cl_kernel = cl::Kernel(m_cl_program, "computeScreenBuffer");
}

int Renderer_OCL::initOCL() {
	// Get list of OpenCL platforms.
	std::vector<cl::Platform> platform;
	cl::Platform::get(&platform);

	if (platform.empty()) {
	    std::cerr << "OpenCL platforms not found." << std::endl;
	    return 1;
	}

	// Get first available GPU device which supports double precision.
	for(auto p = platform.begin(); m_cl_devices.empty() && p != platform.end(); p++) {
	    std::vector<cl::Device> pldev;

	    try {
		p->getDevices(CL_DEVICE_TYPE_GPU, &pldev);

		for(auto d = pldev.begin(); m_cl_devices.empty() && d != pldev.end(); d++) {
		    if (!d->getInfo<CL_DEVICE_AVAILABLE>()) continue;

		    std::string ext = d->getInfo<CL_DEVICE_EXTENSIONS>();

		    if (
			    ext.find("cl_khr_fp64") == std::string::npos &&
			    ext.find("cl_amd_fp64") == std::string::npos
		       ) continue;

		    m_cl_devices.push_back(*d);
		    m_cl_context = cl::Context(m_cl_devices[0]);
		}
	    } catch(...) {
		m_cl_devices.clear();
	    }
	}

	if (m_cl_devices.empty()) {
	    std::cerr << "GPUs with double precision not found." << std::endl;
	    return 1;
	}

	std::cout << m_cl_devices[0].getInfo<CL_DEVICE_NAME>() << std::endl;

	// Create command queue.
	m_cl_queue = cl::CommandQueue(m_cl_context, m_cl_devices[0]);

	compileSources("./kernel/ocl/compute.c");
	makeKernel();

	return 0;
}

struct __attribute__ ((packed)) Camera {
	cl_float3 pos;
	cl_float3 rot;
};

struct __attribute__ ((packed)) DeviceObject {
	cl_float3 pos;
	cl_float3 rot;

	cl_int type_identifier;
	cl_int is_visible;

	
};

float dy = 0;
void Renderer_OCL::updateDy(const float &theta) {
	dy = sin(theta);
}

#define PI 3.14159265359

void Renderer_OCL::render(const vec3f &camera_position, const vec3f &camera_rotation, const std::vector<Object*> &objects) {
	try {
		cl::Buffer device_screen_buffer(m_cl_context, CL_MEM_READ_WRITE, m_width*m_height*sizeof(u32));
//		cl::Buffer device_rendering_plane_x(m_cl_context, CL_MEM_READ_WRITE, m_width*sizeof(float));
//		cl::Buffer device_rendering_plane_y(m_cl_context, CL_MEM_READ_WRITE, m_height*sizeof(float));

//		m_cl_queue.enqueueWriteBuffer(device_rendering_plane_x, CL_TRUE, 0, sizeof(float) * m_width, m_rendering_plane_x);
//		m_cl_queue.enqueueWriteBuffer(device_rendering_plane_y, CL_TRUE, 0, sizeof(float) * m_height, m_rendering_plane_y);
		struct Camera camera = {{ 0, 0, 0 }, { 0, 0, 0 }};


		
		m_cl_kernel.setArg(0, camera);
		m_cl_kernel.setArg(1, static_cast<cl_int>(MAX_STEPS));
		m_cl_kernel.setArg(2, static_cast<cl_float>(MAX_DISTANCE));
		m_cl_kernel.setArg(3, static_cast<cl_float>(EPSILON));
		m_cl_kernel.setArg(4, static_cast<cl_float>(PI/2.f));
		m_cl_kernel.setArg(5, static_cast<cl_float>(m_aspect_ratio));
		m_cl_kernel.setArg(6, static_cast<cl_int>(m_width));
		m_cl_kernel.setArg(7, static_cast<cl_int>(m_height));
		m_cl_kernel.setArg(8, device_screen_buffer);
		m_cl_kernel.setArg(9, static_cast<cl_float>(dy));

		m_cl_queue.enqueueNDRangeKernel(m_cl_kernel, cl::NullRange, cl::NDRange(m_width * m_height), cl::NDRange(std::min(128, m_width * m_height)));
		m_cl_queue.enqueueReadBuffer(device_screen_buffer, CL_TRUE, 0, m_width*m_height*sizeof(u32), m_screen_buffer);
	} catch (const cl::Error &err) {
		std::cerr
			<< "OpenCL error: "
			<< err.what() << "(" << err.err() << ")"
			<< std::endl;
		exit(EXIT_FAILURE);
    	}
}
