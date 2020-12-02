#include <math.h>

#include "graphics/renderer_CPU.h"

#define EPSILON (float) .0001
#define MAX_STEPS 100
#define MAX_DISTANCE 10

float get_distance(const vec3f &pos, const std::vector<Object*> &objects, Object * &closest) {
	float distance = MAX_DISTANCE;

	for (auto it = objects.begin(); it != objects.end(); ++it) {
		if ((*it)->isVisible()) {
			float distance_to_object = (*it)->SDF(pos);
			if (distance_to_object < distance) {
				distance = distance_to_object;
				closest = *it;
			}
		}
	}

	return distance;
}

int sceneSDF(const vec3f &camera_position, const vec3f &ray_step, const std::vector<Object*> &objects, vec3f &hit, float &total_distance, Object * &closest) {
	hit = camera_position;
	total_distance = 0;

	for (int step = 0; step < MAX_STEPS; ++step) {
		
		float distance = get_distance(hit, objects, closest);
		total_distance += distance;

		hit.x += ray_step.x * distance;
		hit.y += ray_step.y * distance;
		hit.z += ray_step.z * distance;	

		if (total_distance >= MAX_DISTANCE) {
			return 0;
		}

		if (distance < EPSILON) {
			return 1;
		}
	}

	return 0;
}

vec3f estimateNormal(const vec3f &pos, const std::vector<Object*> &objects) {
	const vec3f dir = normalize(pos);
	vec3f h;
	Object * o;
	
	float xp, xm;
	float yp, ym;
	float zp, zm;

	sceneSDF({pos.x + EPSILON, pos.y, pos.z}, dir, objects, h, xp, o); sceneSDF({pos.x - EPSILON, pos.y, pos.z}, dir, objects, h, xm, o);
	sceneSDF({pos.x, pos.y + EPSILON, pos.z}, dir, objects, h, yp, o); sceneSDF({pos.x, pos.y - EPSILON, pos.z}, dir, objects, h, ym, o); 
	sceneSDF({pos.x, pos.y, pos.z + EPSILON}, dir, objects, h, zp, o); sceneSDF({pos.x, pos.y, pos.z - EPSILON}, dir, objects, h, zm, o);

	const vec3f normal = { xp - xm, yp - ym, zp - zm };

	return normalize(normal);
}

vec3f phongContribForLight(const vec3f &k_d, const vec3f &k_s, const float &alpha, 
		const vec3f &pos, const vec3f &eye, 
		const vec3f &light_pos, const vec3f &light_intensity, 
		const std::vector<Object*> &objects, const Object * object) {

	const vec3f N = estimateNormal(pos, objects);
	const vec3f L = normalize(vec3f { light_pos.x - pos.x, light_pos.y - pos.y, light_pos.z - pos.z });
	const vec3f V = normalize(vec3f { eye.x - pos.x, eye.y - pos.y, eye.z - pos.z });
	const vec3f R = normalize(reflect({ -L.x, -L.y, -L.z }, N));

	const float dotLN = clamp(dot(L, N), 0.f, 1.f);
	const float dotRV = dot(R, V);

	if (dotLN < 0.0) {
		return { 0, 0, 0 };
	}

	if (dotRV < 0.0) {
		return { 
			dotLN * light_intensity.x * k_d.x, 
			dotLN * light_intensity.y * k_d.y, 
			dotLN * light_intensity.z * k_d.z, 
		};
	}
	
	float s = pow(dotRV, alpha);
	return { 
		light_intensity.x * (k_d.x * dotLN + k_s.x * s), 
		light_intensity.y * (k_d.y * dotLN + k_s.y * s), 
		light_intensity.z * (k_d.z * dotLN + k_s.z * s), 
	};
}

vec3f phongIllumination(const vec3f &k_a, const vec3f &k_d, const vec3f &k_s, const float &alpha, const vec3f &pos, const vec3f &eye, const std::vector<Object*> objects, const Object * object) {
	// TODO: Replace k_a, k_d, k_s and alpha by material map in $object
	// TODO: Replace static lights by lights given in parameters

	const vec3f ambiant_light = { 0.5, 0.5, 0.5 };
	vec3f color = { ambiant_light.x * k_a.x, ambiant_light.y * k_a.y, ambiant_light.z * k_a.z };

	const vec3f light_pos = { 0, 5, 6 };
	const vec3f light_intensity = { .4f, .4f, .4f };

	const vec3f phong = phongContribForLight(k_d, k_s, alpha, pos, eye, light_pos, light_intensity, objects, object);
	color = { color.x + phong.x, color.y + phong.y, color.z + phong.z };

	return color;
}

const mat3f viewMatrix(const vec3f &eye, const vec3f &center, const vec3f &up) {
	vec3f f = normalize(vec3f { center.x - eye.x, center.y - eye.y, center.z - eye.z });
	vec3f s = normalize(cross(f, up));
	vec3f u = cross(s, f);

	return mat3f { s, u, { -f.x, -f.y, -f.z }};
}

template<class T>
void clearBuffer(T * buffer, const int &size) {
	for(int i = 0; i < size; ++i) {
		*(buffer+i) = (T) 0;
	}
}

void Renderer_CPU::render(const vec3f &camera_position, const vec3f &camera_rotation, const std::vector<Object*> &objects) {
	clearBuffer(m_screen_buffer, m_width*m_height);
	clearBuffer(m_depth_buffer, m_width*m_height);

	int pointer = 0;

	const mat3f view_to_world = viewMatrix(camera_position, { 0, 0, 0 }, { 0, 1, 0 });

	for (int y = 0; y < m_height; ++y) {
		for (int x = 0; x < m_width; ++x) {
			const vec3f view_dir = mul(view_to_world, normalize(vec3f { m_rendering_plane_x[x], m_rendering_plane_y[y], m_distance_to_plane }));
			
			vec3f hit;
			Object * closest;
			float total_distance = 0;
			
			int has_hit = sceneSDF(camera_position, view_dir, objects, hit, total_distance, closest);
			m_depth_buffer[pointer] = total_distance;
			
			if (total_distance >= MAX_DISTANCE - EPSILON) {
				u32 final_color = 0xFF << 24 | 0xFF << 16;
				m_screen_buffer[pointer] = final_color;
			} else {
				vec3f pos = {
					camera_position.x + total_distance * view_dir.x,
					camera_position.y + total_distance * view_dir.y,
					camera_position.z + total_distance * view_dir.z,
				};

				vec3f k_a = { 0.2, 0.2, 0.2 };
				vec3f k_d = { 0.7, 0.2, 0.2 };
				vec3f k_s = { 1, 1, 1 };
				float shininess = 10.0;

				vec3f color = phongIllumination(k_a, k_d, k_s, shininess, pos, camera_position, objects, closest);			
				
				int final_color = 0xFF << 24 | ((u8) (color.x * 255)) << 16 | ((u8) (color.y * 255)) << 8 | (u8) (color.z * 255);
				
				m_screen_buffer[pointer] = final_color;
			}

			++pointer;
		}
	}
}
