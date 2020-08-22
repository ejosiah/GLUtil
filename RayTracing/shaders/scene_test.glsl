#pragma include("ray_tracing_model.glsl")
#pragma include("ray_shape_test.glsl")
#pragma include("ray_triangle_test.glsl")

bool anyHit(Ray ray) {
	HitInfo local_hit;
	local_hit.t = ray.tMax;

	if (intersectCube(ray, bounds, local_hit)) {
		for (int i = 0; i < numSpheres; i++) {
			Sphere s = sphere[i];
			if (intersectSphere(ray, s, local_hit)) {
				return true;
			}
		}

		local_hit.t = ray.tMax;
		if (intersectsTriangleAnyHit(ray, local_hit, 0)) {
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

	if (intersectCube(ray, bounds, local_hit)) {
		atomicCounterIncrement(test_count);
		//hit = local_hit;
		//return true;
		for (int i = 0; i < numSpheres; i++) {
			Sphere s = sphere[i];
			atomicCounterIncrement(test_count);
			if (intersectSphere(ray, s, local_hit)) {
				aHit = true;
				if (local_hit.t < hit.t) {
					hit = local_hit;
				}
			}
		}

		//local_hit.t = hit.t;
		//if (intersectsTriangle(ray, local_hit)) {
		//	aHit = true;
		//	if (local_hit.t < hit.t) {
		//		hit = local_hit;
		//	}
		//}

		local_hit.t = hit.t;
		if (intersectsTriangle(ray, local_hit, 0)) {
			aHit = true;
			if (local_hit.t < hit.t) {
				hit = local_hit;
			}
		}
	}



	//local_hit.t = hit.t;
	//for (int i = 0; i < numTriangles; i++) {
	//	Triangle tri = triangles[i];
	//	if (intersectTriangle(ray, tri, local_hit)) {
	//		aHit = true;
	//		if (local_hit.t < hit.t) {
	//			hit = local_hit;
	//		}
	//	}
	//}

	//local_hit.t = hit.t;
	//if (intersectBox(ray, box, local_hit)) {
	//	aHit = true;
	//	if (local_hit.t < hit.t) {
	//		hit = local_hit;
	//	}
	//}

	local_hit.t = hit.t;
	for (int i = 0; i < numPlanes; i++) {
		atomicCounterIncrement(test_count);
		if (intersectPlane(ray, plane[i], local_hit)) {
			aHit = true;
			if (local_hit.t < hit.t) {
				hit = local_hit;
			}
		}
	}

	return aHit;
}