#ifndef OBJECT_H
#define OBJECT_H

#include <vector>

#include "def.h"

class Object {
	public:
		Object() {
			m_uuid = uuid::generate_uuid_v4();
		}

		void attachChild(Object * child) {
			m_children.push_back(child);
		}

		void detachChild(UUID uuid) {
			
			auto it = m_children.begin();
			for (; it != m_children.end() && (*it)->getUUID() != uuid; ++it)
			if (it < m_children.end()) {
				m_children.erase(it);
			}
		}

		void update(const long int &time) {
			myUpdate(time);
			for (auto it = m_children.begin(); it != m_children.end(); ++it) {
				(*it)->update(time);
			}
		}

		virtual void myUpdate(const long int & time) = 0;

		virtual float SDF(const vec3f &pos) const = 0;

		int isVisible() const {
			return m_is_visible;
		}

		const UUID getUUID() const {
			return m_uuid;
		}

	private:
		UUID m_uuid;
		vec3f m_pos;
		vec3f m_rot;

		std::vector<Object*> m_children;

		int m_is_visible = 1;
};

#endif
