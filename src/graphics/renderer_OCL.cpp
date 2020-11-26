#include "graphics/renderer_OCL.h"

//#define __CL_ENABLE_EXCEPTIONS

#include <iostream>
#include <vector>

Renderer_OCL::Renderer_OCL(const int &width, const int &height, const float &fov) : Renderer(width, height, fov) {
	if (initOCL()) {
		printf("Could not initialize OpenCL!\nExiting...\n");
		exit(EXIT_FAILURE);
	}
}

Renderer_OCL::~Renderer_OCL() {

}

static const char source[] =
    "#if defined(cl_khr_fp64)\n"
    "#  pragma OPENCL EXTENSION cl_khr_fp64: enable\n"
    "#elif defined(cl_amd_fp64)\n"
    "#  pragma OPENCL EXTENSION cl_amd_fp64: enable\n"
    "#else\n"
    "#  error double precision is not supported\n"
    "#endif\n"
    "kernel void add(\n"
    "       ulong n,\n"
    "       global const double *a,\n"
    "       global const double *b,\n"
    "       global double *c\n"
    "       )\n"
    "{\n"
    "    size_t i = get_global_id(0);\n"
    "    if (i < n) {\n"
    "       c[i] = a[i] + b[i];\n"
    "    }\n"
    "}\n";

int Renderer_OCL::initOCL() {
	// Get list of OpenCL platforms.
	std::vector<cl::Platform> platform;
	cl::Platform::get(&platform);

	if (platform.empty()) {
	    std::cerr << "OpenCL platforms not found." << std::endl;
	    return 1;
	}

	// Get first available GPU device which supports double precision.
	std::vector<cl::Device> device;
	for(auto p = platform.begin(); device.empty() && p != platform.end(); p++) {
	    std::vector<cl::Device> pldev;

	    try {
		p->getDevices(CL_DEVICE_TYPE_GPU, &pldev);

		for(auto d = pldev.begin(); device.empty() && d != pldev.end(); d++) {
		    if (!d->getInfo<CL_DEVICE_AVAILABLE>()) continue;

		    std::string ext = d->getInfo<CL_DEVICE_EXTENSIONS>();

		    if (
			    ext.find("cl_khr_fp64") == std::string::npos &&
			    ext.find("cl_amd_fp64") == std::string::npos
		       ) continue;

		    device.push_back(*d);
		    m_cl_context = cl::Context(device);
		}
	    } catch(...) {
		device.clear();
	    }
	}

	m_cl_device = device[0];

	if (device.empty()) {
	    std::cerr << "GPUs with double precision not found." << std::endl;
	    return 1;
	}

	std::cout << device[0].getInfo<CL_DEVICE_NAME>() << std::endl;

	// Create command queue.
	m_cl_queue = cl::CommandQueue(m_cl_context, device[0]);

	m_cl_program = cl::Program(m_cl_context, cl::Program::Sources(
		    1, std::make_pair(source, strlen(source))
		    ));

	try {
	    m_cl_program.build(device);
	} catch (const cl::Error&) {
	    std::cerr
		<< "OpenCL compilation error" << std::endl
		<< m_cl_program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device[0])
		<< std::endl;
	    exit(EXIT_FAILURE);
	}

	m_cl_kernel = cl::Kernel(m_cl_program, "add");

	return 0;
}




void Renderer_OCL::render(const vec3f &camera_position, const vec3f &camera_rotation, const std::vector<Object*> &objects) {

    const size_t N = 1 << 20;

    try {

	// Compile OpenCL program for found device.


	// Prepare input data.
	std::vector<double> a(N, 1);
	std::vector<double> b(N, 2);
	std::vector<double> c(N);

	// Allocate device buffers and transfer input data to device.
	cl::Buffer A(m_cl_context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
		a.size() * sizeof(double), a.data());

	cl::Buffer B(m_cl_context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
		b.size() * sizeof(double), b.data());

	cl::Buffer C(m_cl_context, CL_MEM_READ_WRITE,
		c.size() * sizeof(double));

	// Set kernel parameters.
	m_cl_kernel.setArg(0, static_cast<cl_ulong>(N));
	m_cl_kernel.setArg(1, A);
	m_cl_kernel.setArg(2, B);
	m_cl_kernel.setArg(3, C);
	
	// Launch kernel on the compute device.
	m_cl_queue.enqueueNDRangeKernel(m_cl_kernel, cl::NullRange, cl::NDRange(N), cl::NDRange(256));

	// Get result back to host.
	m_cl_queue.enqueueReadBuffer(C, CL_TRUE, 0, c.size() * sizeof(double), c.data());

	int check = 3;
	int test = 1;
	for (size_t i = 0; test && i < 1<<20; ++i) {
		test = ((c[i] == check) ? 1 : 0);
	}

	// Should get '3' here.
/*	if (check) {
		std::cout << "YAY" << std::endl;
	} else {
		std::cout << "NOO" << std::endl;
	}*/
    } catch (const cl::Error &err) {
	std::cerr
	    << "OpenCL error: "
	    << err.what() << "(" << err.err() << ")"
	    << std::endl;
	exit(EXIT_FAILURE);
    }
}