#include <stdio.h>
#include <stdlib.h>

#include "scene.h"

int main(int argc, char ** argv) {
	Scene scene;
	Renderer renderer(1920, 1080);
	scene.attachRenderer(&renderer);

	return 0;
}
