#include "window.h"

Window::Window(const int &width, const int &height) {
	m_width = width;
	m_height = height;
}

Window::~Window() {
	cleanup();
}

int Window::init(const char * title) {
	if (SDL_Init(SDL_INIT_EVERYTHING)) {
		printf("SDL could not initialize!\nSDL_Error: %s\n", SDL_GetError());
		return 0;
	}

	m_window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_width, m_height, SDL_WINDOW_SHOWN);
	if (m_window == NULL) {
		printf("Window could not be created!\n SDL_Error: %s\n", SDL_GetError());
		cleanup();
		return 0;
	}

	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
	if (m_renderer == NULL) {
		printf("Renderer could not be created!\n SDL_Error: %s\n", SDL_GetError());
		cleanup();
		return 0;
	}

	m_frame = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, m_width, m_height);
	if (m_frame == NULL) {
		printf("Frame could not be created!\n SDL_Error: %s\n", SDL_GetError());
		cleanup();
		return 0;
	}

	return 1;
}


void Window::cleanup() {
	if (m_frame != NULL) {
		SDL_DestroyTexture(m_frame);
		m_frame = NULL;
	}

	if (m_renderer == NULL) {
		SDL_DestroyRenderer(m_renderer);
		m_renderer = NULL;
	}

	if (m_window != NULL) {
		SDL_DestroyWindow(m_window);
		m_window = NULL;
	}

	SDL_Quit();
}

void Window::updateFrameBuffer(const u32 * buffer) {
	SDL_RenderClear(m_renderer);

	u32 * pixel_buffer;
	int pitch = 0;

	if (!SDL_LockTexture(m_frame, NULL, (void**) &pixel_buffer, &pitch)) {
		pitch /= sizeof(u32);
		memcpy(pixel_buffer, buffer, m_width*m_height*sizeof(u32));
		SDL_UnlockTexture(m_frame);
		SDL_RenderCopy(m_renderer, m_frame, NULL, NULL);
		SDL_RenderPresent(m_renderer);
	} else {
		printf("Could not render screen buffer!\nSDL_Error: %s\n", SDL_GetError());
	}

	SDL_Delay(7);
}
