#ifndef RENDERER_H
#define RENDERER_H

#include <unordered_map>

#include "def.h"
#include "object.h"

class Renderer {
	public:
		typedef char u8;
		typedef char* UUID;

		Renderer(const int &width, const int &height);
		~Renderer();

		void render(const std::unordered_map<UUID, Object*> &objects);

		u8 * getScreenBuffer() const;

	private:
		int m_width, m_height;
		double * m_depth_buffer;
		u8 * m_screen_buffer;
};

#endif
