#ifndef SCENE_H
#define SCENE_H

#include <unordered_map>
#include <vector>

#include "def.h"
#include "object.h"
#include "renderer.h"

typedef char* UUID;

class Scene {
	public:
		Scene();

		void attachRenderer(Renderer * renderer);

		void update(const long int &time);
		void render();

		void addObject(Object * object);
		void removeObject(UUID uuid);

	private:
		std::unordered_map<UUID, Object*> m_objects;
		std::vector<UUID> m_objects_uuid;

		Renderer * m_renderer;
};

#endif
