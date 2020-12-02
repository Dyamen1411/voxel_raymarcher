#if defined(cl_khr_fp64)
#	pragma OPENCL EXTENSION cl_khr_fp64: enable
#elif defined(cl_amd_fp64)
#	pragma OPENCL EXTENSION cl_amd_fp64: enable
#elif
#	error double precision is not supported
#endif

/*
 * What I need:
 * 	Objects
 * 	Lights
 * 	Camera Pos & Rot
 * 	normalized raydir untransformed (x y)
 */

struct Material {
	//TODO: implement
	float3 k_a;
	float3 k_d;
	float3 k_s;
	float alpha;
};

struct Object {
	// Position and rotaion
	float3 pos;
	float3 rot;

	int type_identifier;
	int is_visible;

	struct Material material;

	// Attributes
	void * attributes;
};

struct __attribute__ ((packed)) Camera {
	float3 pos;
	float3 rot;
};

float sphereSDF(const float3 pos,
		const struct Object o) {

	const float radius = (float) *((float*) o.attributes);
	return length(o.pos - pos) - radius;
}

// TODO: Transform this function into an array and for a given type return pointer to correct SDF
float SDF(const float3 pos,
		const float max_distance,
		const struct Object object) {

	float distance = max_distance;
	if (object.type_identifier == 0) {
		distance = sphereSDF(pos, object);
	} else {
		// can't render yet :(
	}

	return distance;
}

float getDistance(const float3 pos, 
		const struct Object * objects, const int object_count,
		struct Object * closest,
		const float max_distance) {

	float distance = max_distance;

	for (int i = 0; i < object_count; ++i) {
		struct Object obj = objects[i];
		if (obj.is_visible) {
			float distance_to_object = SDF(pos, max_distance, obj);
			if (distance_to_object < distance) {
				distance = distance_to_object;
				*closest = obj;
			}
		}
	}

	return distance;
}

int sceneSDF(const float3 pos, const float3 dir, 
		const struct Object * objects, const int object_count,
		float3 * hit, float * total_distance, struct Object * closest, 
		const int max_steps, const float max_distance, const float epsilon) {

	*hit = pos;
	*total_distance = 0;

	for (int i = 0; i < max_steps; ++i) {
		float distance = getDistance(*hit, objects, object_count, closest, max_distance);
		*hit += dir * distance;
		*total_distance += distance;

		if (*total_distance >= max_distance - epsilon) {
			return 0;
		}

		if (distance < epsilon) {
			return 1;
		}
	}

	return 0;
}

float3 estimateNormal(const float3 pos, 
		const struct Object * objects, const int object_count, 
		const int max_steps, const float max_distance, const float epsilon) {

		const float3 dir = normalize(pos);
		float3 h;
		struct Object o;

		float xp, xm;
		float yp, ym;
		float zp, zm;

		const float3 xo = { epsilon, 0, 0 };
		const float3 yo = { 0, epsilon, 0 };
		const float3 zo = { 0, 0, epsilon };

		sceneSDF(pos + xo, dir, objects, object_count, &h, &xp, &o, max_steps, max_distance, epsilon);
		sceneSDF(pos - xo, dir, objects, object_count, &h, &xm, &o, max_steps, max_distance, epsilon);
		sceneSDF(pos + yo, dir, objects, object_count, &h, &yp, &o, max_steps, max_distance, epsilon);
		sceneSDF(pos - yo, dir, objects, object_count, &h, &ym, &o, max_steps, max_distance, epsilon);
		sceneSDF(pos + zo, dir, objects, object_count, &h, &zp, &o, max_steps, max_distance, epsilon);
		sceneSDF(pos - zo, dir, objects, object_count, &h, &zm, &o, max_steps, max_distance, epsilon);

		const float3 normal = { xp - xm, yp - ym, zp - zm };

		return normalize(normal);
}

float3 reflect(const float3 a, const float3 b) {
	return a - 2.f*dot(a, b)*b;
}

float3 phongContribForLight(const float3 pos, const float3 eye,
		const float3 light_pos, const float3 light_intensity,
		const struct Object object, const struct Object * objects, const int object_count,
		const int max_steps, const float max_distance, const float epsilon) {

	const float3 N = estimateNormal(pos, objects, object_count, max_steps, max_distance, epsilon);
	const float3 L = normalize(light_pos - pos);
	const float3 V = normalize(eye - pos);
	const float3 R = normalize(reflect(-L, N));

	const float dotLN = clamp(dot(L, N), 0.f, 1.f);
	const float dotRV = dot(R, V);
	
	float3 contrib = { 0, 0, 0 };
	if (dotLN >= 0 && dotRV < 0) {
		contrib = dotLN * object.material.k_d * light_intensity;
	} else {
		contrib = light_intensity * (object.material.k_d * dotLN * object.material.k_s * pow(dotRV, object.material.alpha));
	}

	return contrib;
}

float3 phongIllumination(const float3 pos, const float3 eye,
		const struct Object object, const struct Object * objects, const int object_count,
		const int max_steps, const float max_distance, const float epsilon) {

	const float3 ambiant_light = { 0.5, 0.5, 0.5 };
	float3 color = ambiant_light * object.material.k_a;

	const float3 light_pos = { 0, 5, 6 };
	const float3 light_intensity = { 0.4, 0.4, 0.4 };

	color += phongContribForLight(pos, eye, light_pos, light_intensity, object, objects, object_count, max_steps, max_distance, epsilon);

	return color;
}

/* Lights will be static for now
 * TODO: Pass the lights as parameters
 * TODO: Make objects have a material instead of harcode output color
 */
kernel 
void computeScreenBuffer(//global const Object * objects, const int object_count,
		/*global const Light * lights,*/
		const struct Camera camera,
//		global const Object * objects,
		const int max_steps, const float max_distance, const float epsilon,
		const float fov, const float aspect_ratio,  const int width, const int height, 
		global int * screen_buffer,
		const float dy) {

	size_t index = get_global_id(0);
	size_t x = index % width;
	size_t y = index / width;
	
	if (x >= width || y >= height) {
		return;
	}
	
//	const float3 view_dir = { rendering_plane_x[x], rendering_plane_y[y], distance_to_plane };
//	const float3 normalized_view_dir = normalize( view_dir );

	const float distance_to_plane = 1.f / tan(fov/2.f);
	const float ray_dir_x = 2.f * aspect_ratio * (( ((float) x) / ((float) width - 1)) - .5f);
	const float ray_dir_y = 2.f * ((((float) y) / ((float) height - 1)) - .5f);

	float3 view_dir = { ray_dir_x, ray_dir_y, distance_to_plane };
	view_dir = normalize(view_dir);

	const float3 camera_pos = { 0, 0, 0 };

	float3 hit;
	struct Object closest;
	float total_distance;
	
	struct Material mat = {
		{ 0.2, 0.2, 0.2 },
		{ 0.7, 0.2, 0.2 },
		{ 1.0, 1.0, 1.0 },
		10.0
	};

	float radius[1] = { 1 };

	struct Object objects[2] = { 
		{{  1, dy, 5 }, { 0, 0, 0 }, 0, 1, mat, (void*) radius },
		{{ -1, dy, 5 }, { 0, 0, 0 }, 0, 1, mat, (void*) radius } 
	};

	int has_hit = sceneSDF(camera_pos, view_dir, objects, 2, &hit, &total_distance, &closest, max_steps, max_distance, epsilon);
	//TODO: implements depth buffer
	
	int color;
	if (has_hit) {
		color = 0xFF000000;
		const float3 pos = camera.pos + total_distance * view_dir;
		const float3 c = phongIllumination(pos, camera.pos, closest, objects, 2, max_steps, max_distance, epsilon);
		
		color |= ((int) (c.x * 255)) << 16;
		color |= ((int) (c.y * 255)) <<  8;
		color |= ((int) (c.z * 255)) <<  0;
	} else {

		float3 c = { (view_dir.x + 1) / 2., (view_dir.y + 1) / 2., 0 };
		color = 0xFF000000;
		color |= ((int) (c.x * 255)) << 16;
		color |= ((int) (c.y * 255)) <<  8;
	}

	screen_buffer[index] = color;
}
