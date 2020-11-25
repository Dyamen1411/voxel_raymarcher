#include "graphics/renderer.h"

Renderer::Renderer(const int &width, const int &height, const float &fov) {
        m_width = width;
        m_height = height;

	m_depth_buffer = (double*) malloc(m_width * m_height * sizeof(double));
	m_screen_buffer = (u32*) malloc(m_width * m_height * sizeof(u32));

        m_rendering_plane_x = (float*) malloc(m_width * sizeof(float));
        m_rendering_plane_y = (float*) malloc(m_height * sizeof(float));
        
        m_aspect_ratio = (float) m_width / (float) m_height;

        updateFOV(fov);
}

Renderer::~Renderer() {
        free(m_depth_buffer);
        free(m_screen_buffer);

        free(m_rendering_plane_x);
        free(m_rendering_plane_y);
}

void Renderer::updateFOV(const float &fov) {
        m_distance_to_plane = 1. / tanf(fov/2.);
        updateRenderingPlane();
}

void Renderer::updateRenderingPlane() {
        for (int x = 0; x < m_width; ++x) {
                m_rendering_plane_x[x] = m_aspect_ratio * ((float) x / ((float) m_width - 1) - .5);
        }
        
        for (int y = 0; y < m_height; ++y) {
                m_rendering_plane_y[y] = (float) y / ((float) m_height - 1) - .5;
        }
}

u32 * Renderer::getScreenBuffer() const {
        return m_screen_buffer;
}
