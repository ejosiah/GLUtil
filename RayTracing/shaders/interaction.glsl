#pragma include("ray_tracing_model.glsl")
#pragma include("reflection.glsl")

void intialize(HitInfo hit, Ray ray, out SurfaceInteration interact) {

	switch (hit.shape) {
	case SPHERE_SHAPE: {
		Sphere s = sphere[hit.id];
		Ray r = transform(s.worldToObject, ray);
		vec3 p = r.origin.xyz + r.direction.xyz * hit.t;
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
		interact.n2 = material[interact.matId].ior;

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
		interact.p = ray.origin.xyz + ray.direction.xyz * hit.t;
		interact.n = pl.n;
		interact.color = vec4(0.3, 0.3, 0.3, 1);
		interact.matId = pl.matId;
		interact.shape = PLANE;
		interact.shapeId = hit.id;
		interact.n1 = 1.0;
		interact.n2 = material[interact.matId].ior;

		float whole = 50;

		vec3 p = interact.p;
		interact.uv = vec2(p.x / whole, p.z / whole);
		break;
	}
	}
}

bool hasFlag(SurfaceInteration interact, int expectedTypes) {
	Material material = material[interact.matId];
	for (int i = 0; i < material.nBxDfs; i++) {
		if (matchesFlag(expectedTypes, material.bsdf[i])) {
			return true;
		}
	}
	return false;
}

bool hasBsdf(SurfaceInteration interact, int bsdf) {
	Material material = material[interact.matId];
	for (int i = 0; i < material.nBxDfs; i++) {
		if (isBsdf(bsdf, material.bsdf[i])) {
			return true;
		}
	}
	return false;
}