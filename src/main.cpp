#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "graphics/renderer_CPU.h"
#include "scene.h"
#include "sphere.h"
#include "window.h"

#define W 1920
#define H 1080

int main(int argc, char ** argv) {
	
	Window window(W, H);
	window.init("test");

	Renderer_CPU renderer(W, H, 90);

	Scene scene;
	scene.attachRenderer(&renderer);

	Sphere sphere1(1, {0, 0, -5});
	Sphere sphere2(1, {0, 0,  5});

	{
		std::vector<Object*> identifiers = *Object::getIdentifiers();
		int i = 0;
		printf("Identifiers:\n");
		for (auto v = identifiers.begin(); v != identifiers.end(); ++v) {
			printf("Identifier %02d: %s\n", i, (*v)->getUUID().c_str());
			++i;
		}
	}

	scene.addObject(&sphere1);
	scene.addObject(&sphere2);

	SDL_Event e;
	int exit = 0;
	int use_lock_texture = 1;
	
	float theta = 0;

	while (!exit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				exit = 1;
			}
		}
		
		printf("rendering...\n");
		scene.render({0, sin(theta)/2.f, -1}, {0, 0, 0});
		u32 * screen_buffer = scene.getRenderer()->getScreenBuffer();
		printf("rendered!\n");
		
		window.updateFrameBuffer(screen_buffer);
		
		theta += .1f;
	}

	window.cleanup();

	return 0;
}
