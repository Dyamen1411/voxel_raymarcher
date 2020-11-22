#include "renderer.h"

Renderer::Renderer(const int &width, const int &height) {
	m_depth_buffer = (double*) malloc(width * height * sizeof(double));
	m_screen_buffer = (u8*) malloc(width * height * 3);
}

Renderer::~Renderer() {
	free(m_depth_buffer);
	free(m_screen_buffer);
}

void Renderer::render(const std::unordered_map<UUID, Object*> &objects) {

}

u8 * Renderer::getScreenBuffer() const {
	return m_screen_buffer;
}
