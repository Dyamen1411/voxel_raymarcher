#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL.h>
#include <vector>

#include "def.h"

class Window {
	public:
		Window(const int &width, const int &height);
		~Window();

		void cleanup();

		int init(const char * title);
		
		void updateFrameBuffer(const u32 * buffer);

	private:
		int m_width, m_height;
		SDL_Window * m_window;
		SDL_Renderer * m_renderer;
		SDL_Texture * m_frame;
};

#endif
