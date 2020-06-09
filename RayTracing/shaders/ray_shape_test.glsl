#pragma include("ray_tracing_model.glsl")

bool intersectPlane(Ray ray, Plane p, out HitInfo hit) {

	float t = p.d - dot(p.n, ray.origin.xyz);
	t /= dot(p.n, ray.direction.xyz);

	if (t < 0 || t > ray.tMax) {
		return false;
	}
	hit.t = t;
	hit.id = p.id;
	hit.shape = PLANE;
	return true;
}

bool intersectSphere(Ray ray, Sphere s, out HitInfo hit) {
	float t0 = -1;
	float t1 = t0;

	Ray r = transform(s.worldToObject, ray);
//	Ray r = ray;

	vec3 m = r.origin.xyz - s.c.xyz;
	float a = dot(r.direction.xyz, r.direction.xyz);
	float b = dot(m, ray.direction.xyz);
	float c = dot(m, m) - s.r * s.r;

	if (c > 0.0 && b > 0.0) return false;

	float discr = b * b - a * c;

	if (discr < 0.0) return false;
	float sqrtDiscr = sqrt(discr);

	t0 = (-b - sqrtDiscr) / a;
	t1 = (-b + sqrtDiscr) / a;
	if (t0 > t1) swap(t0, t1);

	float tHit = t0;
	if (tHit <= 0.0) tHit = t1;
	if (tHit > r.tMax) return false;

	vec3 p = r.origin.xyz + r.direction.xyz * tHit;
	p *= s.r / distance(p, s.c.xyz);
	if (p.x == 0 && p.z == 0) p.x = 1E-5 * s.r;
	float phi = atan(p.z, p.x);
	if (phi < 0.0) phi += TWO_PI;

	if ((s.yMin > -s.r && p.y < s.yMin) || (s.yMax < s.r && p.y > s.yMax) || phi > s.phiMax) {
		if (tHit == t1) return false;
		if (t1 > ray.tMax) return false;

		tHit = t1;
		p = r.origin.xyz + r.direction.xyz * tHit;
		p *= s.r / distance(p, s.c.xyz);
		if (p.x == 0.0 && p.z == 0.0) p.x = 1E-5 * s.r;
		float phi = atan(p.z, p.x);
		if (phi < 0.0) phi += TWO_PI;

		if ((s.yMin > -s.r && p.y < s.yMin)
			|| (s.yMax < s.r && p.y > s.yMax)
			|| phi > s.phiMax) return false;
	}

	hit.t = tHit;
	hit.shape = SPHERE_SHAPE;
	hit.id = s.id;
	hit.extras.x = phi;
	return true;
}

bool hasNoVolume(Box box) {
	return all(equal(box.min, vec3(0))) && all(equal(box.max, vec3(0)));
}

bool intersectBox(Ray ray, Box box) {
	if (hasNoVolume(box)) return false;
	vec3   tMin = (box.min - ray.origin.xyz) / ray.direction.xyz;
	vec3   tMax = (box.max - ray.origin.xyz) / ray.direction.xyz;
	vec3     t1 = min(tMin, tMax);
	vec3     t2 = max(tMin, tMax);
	float tNear = max(max(t1.x, t1.y), t1.z);
	float  tFar = min(min(t2.x, t2.y), t2.z);
	return  tNear < ray.tMax || tNear < tFar;
}