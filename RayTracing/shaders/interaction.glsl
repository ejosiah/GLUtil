#pragma include("ray_tracing_model.glsl")
#pragma include("ray_triangle_test.glsl")
#pragma include("reflection.glsl")

const int FRONT_FACE = 0;
const int BACK_FACE = 1;

void intialize(HitInfo hit, Ray ray, out SurfaceInteration interact) {

	switch (hit.shape) {
	case SPHERE_SHAPE: {
		Sphere s = sphere[hit.id];
		Ray r = transform(s.worldToObject, ray);
		vec3 p = r.origin + r.direction * hit.t;
		p *= s.r / distance(p, s.c.xyz);
		vec3 n = p - s.c.xyz;
		float phi = atan(p.z, p.x);
		float u = phi / s.phiMax;
		float theta = acos(clamp(p.y / s.r, -1, 1));
		float v = (theta - s.thetaMin) / (s.thetaMax - s.thetaMin);
		mat4 otw = s.objectToWorld;

		interact.p = (otw * vec4(p, 1)).xyz;
		interact.n = normalize(mat3(otw) * n);
		interact.uv = vec2(u, v);
		interact.color = vec4(0, 1, 0, 1);
		interact.matId = s.matId;
		interact.shape = SPHERE_SHAPE;
		interact.n1 = 1.0;
	//	interact.n2 = material[interact.matId].ior;
		interact.n2 = 2.4;
		interact.shapeId = hit.id;

		// calculate shading data
		float yRadius = length(p.xz);
		float invYRaduis = 1.0 / yRadius;
		float cosPhi = p.x * invYRaduis;
		float sinPhi = p.y * invYRaduis;

		vec3 dpdu = vec3(-s.phiMax * p.z, 0, s.phiMax * p.x);
		vec3 dpdv = (s.thetaMax - s.thetaMin) * vec3(p.y * cosPhi, s.r * sin(theta), p.y * sinPhi);

		//vec3 d2Pduu = -s.phiMax * s.phiMax * vec3(p.x, 0, p.z);
		//vec3 d2Pduv = (s.thetaMax - s.thetaMin) * p.y * s.phiMax * vec3(-sinPhi, 0, cosPhi);
		//vec3 d2Pdvv = -(s.thetaMax - s.thetaMin) * (s.thetaMax - s.thetaMin) * p;

		//float E = dot(dpdu, dpdu);
		//float F = dot(dpdu, dpdv);
		//float G = dot(dpdv, dpdv);

		//vec3 N = normalize(cross(dpdu, dpdv));
		//float e = dot(n, d2Pduu);
		//float f = dot(n, d2Pduv);
		//float g = dot(n, d2Pdvv);
		//
		//float invEGF2 = 1.0/(E * G - F * F);
		//vec3 dndu = vec3((f * F - e * G) * invEGF2 * dpdu + (e * F - f * E) * invEGF2 * dpdv);
		//vec3 dndv = vec3((g * F - f * G) * invEGF2 * dpdu + (f * F - g * E) * invEGF2 * dpdv);

		break;
	}
	case PLANE: {
		Plane pl = plane[hit.id];
		interact.p = pointOnRay(ray, hit);
		interact.n = pl.n.xyz;
		interact.color = vec4(0.3, 0.3, 0.3, 1);
		interact.matId = pl.matId;
		interact.shape = PLANE;
		interact.shapeId = hit.id;
		interact.n1 = 1.0;
		interact.n2 = material[interact.matId].ior;

		float whole = 10;

		vec3 p = interact.p;
		vec3 n = interact.n;

	//	n = dot(-ray.direction, n) < 0 ? -n : n;

		vec3 x, y;
		orthonormalBasis(n, x, y);
		
	//	interact.uv = vec2(p.x / whole, p.z / whole);
		interact.uv = vec2(dot(p, x)/whole, dot(p, y)/whole);
		break;
	}
	case BOX: {
		vec3 t1 = hit.extras.xyz;

		vec3 p = pointOnRay(ray, hit);
		vec3 n = -sign(ray.direction) * step(t1.yzx, t1.xyz) * step(t1.zxy, t1.xyz);
		vec3 x, y;
		orthonormalBasis(n, x, y);

		interact.p = p;
		interact.n = n;
		interact.uv = vec2(dot(p, x)/5, dot(p, y)/5);
		interact.color = vec4(1, 0, 0, 1);	
		interact.matId = -1;
		interact.shape = BOX;
		break;
	}
	case TRIANGLE: {
		Triangle tri;
		Shading s;
		fetchShading(hit.id, tri, s);
		float u = hit.extras.x;
		float v = hit.extras.y;
		float w = hit.extras.z;
		vec3 p = ray.origin + ray.direction * hit.t;
		vec3 n = (s.n0 * u + s.n1 * v + s.n2 * w) * hit.extras.w;
		
		if (!isFrontFacing(tri, ray.origin)) {
			swap(u, w);
		}

		vec2 uv = s.uv0 * u + s.uv1 * v + s.uv2 * w;

		interact.p = p;
		interact.n = n;
		interact.uv = uv;
		interact.color = vec4(0.1, 0.1, 0.1, 1);
		interact.matId = tri.matId;
		interact.shape = TRIANGLE;
		interact.n1 = 1.0;
		interact.n2 = material[interact.matId].ior;
		break;
	}
	}
}

bool hasFlag(SurfaceInteration interact, int expectedTypes) {
	if (interact.matId < 0) return false;
	Material material = material[interact.matId];
	for (int i = 0; i < material.nBxDfs; i++) {
		if (matchesFlag(expectedTypes, material.bsdf[i])) {
			return true;
		}
	}
	return false;
}

bool hasBsdf(SurfaceInteration interact, int bsdf) {
	if (interact.matId < 0) return false;
	Material material = material[interact.matId];
	for (int i = 0; i < material.nBxDfs; i++) {
		if (isBsdf(bsdf, material.bsdf[i])) {
			return true;
		}
	}
	return false;
}