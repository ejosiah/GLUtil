#pragma include("ray_tracing_model.glsl")

bool intersectPlane(Ray ray, Plane p, out HitInfo hit) {

	float t = p.d - dot(p.n.xyz, ray.origin.xyz);
	t /= dot(p.n.xyz, ray.direction.xyz);

	if (t < 0 || t > ray.tMax) {
		return false;
	}


	hit.t = t;
	hit.id = p.id;
	hit.shape = PLANE;

	vec3 o = pointOnRay(ray, hit);
	
	if (o.x > p.max.x || o.y > p.max.y || o.z > p.max.z) return false;
	if (o.x < p.min.y || o.y < p.min.y || o.z < p.min.z) return false;
	
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

bool intersectCube(Ray ray, Box box, out HitInfo hit) {
	if (hasNoVolume(box)) return false;
	vec3   tMin = (box.min - ray.origin.xyz) / ray.direction.xyz;
	vec3   tMax = (box.max - ray.origin.xyz) / ray.direction.xyz;
	vec3     t1 = min(tMin, tMax);
	vec3     t2 = max(tMin, tMax);
	float tn = max(max(t1.x, t1.y), t1.z);
	float tf = min(min(t2.x, t2.y), t2.z);

	if (tn > ray.tMax) return false;

	//	if(tn < tf){
	//		if(!testOnly){
	//			interation.p = ray.o + ray.d * tn;
	//			interation.n = -sign(ray.d) * step(t1.yzx, t1.xyz) * step(t1.zxy, t1.xyz);
	//		}
	//		return true;
	//	}

	hit.t = tn;
	hit.extras = vec4(t1, tf);
	hit.shape = BOX;

	return tn < tf;
}


//bool triangleRayIntersect(Ray ray, Triangle tri, out float t, out float u, out float v, out float w) {
//	vec3 ba = tri.b.xyz - tri.a.xyz;
//	vec3 ca = tri.c.xyz - tri.a.xyz;
//	vec3 pa = ray.origin.xyz - tri.a.xyz;
//	vec3 pq = -ray.direction.xyz;
//
//	vec3 n = cross(ba, ca);
//	float d = dot(pq, n);
//
//	if (d <= 0) return false; // ray is either coplainar with triangle abc or facing opposite it
//
//	t = dot(pa, n);
//
//	if (t < 0) return false;     // ray invariant t >= 0
//
//	vec3 e = cross(pq, pa);
//
//	v = dot(e, ca);
//	if (v < 0.0f || v > d) return false;
//
//	w = -dot(e, ba);
//	if (w < 0.0f || (v + w) > d) return false;
//
//	float ood = 1.0 / d;
//
//	t *= ood;
//	v *= ood;
//	w *= ood;
//	u = 1 - v - w;
//
//	return true;
//}