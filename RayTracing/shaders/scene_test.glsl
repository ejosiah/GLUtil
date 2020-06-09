#pragma include("ray_tracing_model.glsl")
#pragma include("ray_shape_test.glsl")

bool anyHit(Ray ray) {
	HitInfo local_hit;
	local_hit.t = ray.tMax;
	for (int i = 0; i < numSpheres; i++) {
		Sphere s = sphere[i];
		if (intersectSphere(ray, s, local_hit)) {
			return true;
		}
	}

	local_hit.t = ray.tMax;
	if (intersectPlane(ray, plane[0], local_hit)) {
		return true;
	}

	return false;
}

bool intersectScene(Ray ray, out HitInfo hit) {
	hit.t = ray.tMax;
	HitInfo local_hit;
	local_hit.t = hit.t;
	bool aHit = false;
	for (int i = 0; i < numSpheres; i++) {
		Sphere s = sphere[i];
		if (intersectSphere(ray, s, local_hit)) {
			aHit = true;
			if (local_hit.t < hit.t) {
				hit = local_hit;
			}
		}
	}

	//local_hit.t = hit.t;
	//if (intersectsTriangle(ray, local_hit, 0)) {
	//	aHit = true;
	//	if (local_hit.t < hit.t) {
	//		hit = local_hit;
	//	}
	//}

	local_hit.t = hit.t;
	if (intersectPlane(ray, plane[0], local_hit)) {
		aHit = true;
		if (local_hit.t < hit.t) {
			hit = local_hit;
		}
	}

	return aHit;
}