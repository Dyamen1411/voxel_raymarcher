#include "scene.h"

#include <stdio.h>

Scene::Scene() {

}

void Scene::attachRenderer(Renderer * renderer) {
	m_renderer = renderer;
}

void Scene::update(const long int &time) {
	for (auto it = m_objects.begin(); it != m_objects.end(); ++it) {
		(*it)->update(time);
	}
}

void Scene::render(const vec3f &camera_position, const vec3f &camera_rotation) const {
	m_renderer->render(camera_position, camera_rotation, m_objects);
}

void Scene::addObject(Object * object) {
	m_objects.push_back(object);
}

void Scene::removeObject(UUID uuid) {
	//TODO: Implement
}

Renderer * Scene::getRenderer() {
	return m_renderer;
}
