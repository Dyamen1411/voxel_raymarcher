#ifndef SCENE_H
#define SCENE_H

#include <vector>

#include "def.h"
#include "object.h"
#include "renderer.h"

class Scene {
	public:
		Scene();

		void attachRenderer(Renderer * renderer);

		void update(const long int &time);
		void render(const vec3f &camera_position, const vec3f &camera_rotation) const;

		void addObject(Object * object);
		void removeObject(UUID uuid);

		Renderer * getRenderer();

	private:
		std::vector<Object*> m_objects;

		Renderer * m_renderer;
};

#endif
