#include "scene.h"

#include <stdio.h>

Scene::Scene() {

}

void Scene::attachRenderer(Renderer * renderer) {
	m_renderer = renderer;
}

void Scene::update(const long int &time) {

}

void Scene::render() {
	m_renderer->render(m_objects);
}

void Scene::addObject(Object * object) {

}

void Scene::removeObject(UUID uuid) {

}
