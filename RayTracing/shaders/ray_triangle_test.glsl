#pragma include("ray_tracing_model.glsl")



bool triangleRayIntersect(Ray ray, Triangle tri, out float t, out float u, out float v, out float w) {
	vec3 ba = tri.b.xyz - tri.a.xyz;
	vec3 ca = tri.c.xyz - tri.a.xyz;
	vec3 pa = ray.origin - tri.a.xyz;
	vec3 pq = -ray.direction;

	vec3 n = cross(ba, ca);
	float d = dot(pq, n);

	if (d <= 0) return false; // ray is either coplainar with triangle abc or facing opposite it

	t = dot(pa, n);

	if (t < 0) return false;     // ray invariant t >= 0

	vec3 e = cross(pq, pa);

	v = dot(e, ca);
	if (v < 0.0f || v > d) return false;

	w = -dot(e, ba);
	if (w < 0.0f || (v + w) > d) return false;

	float ood = 1.0 / d;

	t *= ood;
	v *= ood;
	w *= ood;
	u = 1 - v - w;

	return true;
}

bool isFrontFacing(Triangle triangle, vec3 reference) {
	vec3 a = triangle.a.xyz;
	vec3 b = triangle.b.xyz;
	vec3 c = triangle.c.xyz;
	vec3 d = reference;

	return dot(a - d, cross(b - d, c - d)) < 0;
}


bool intersectTriangle(Ray ray, Triangle tri, out HitInfo hit) {

	int flip = 1;
	if (!cullBackFace && !isFrontFacing(tri, ray.origin)) {
		swap(tri.a, tri.c);
		flip = -1;
	}

	float t, u, v, w;
	bool intersects = triangleRayIntersect(ray, tri, t, u, v, w);
	hit.t = t;
	hit.shape = TRIANGLE;
	hit.id = tri.id;
	hit.extras = vec4(u, v, w, flip);
	return intersects;
}

#pragma include("cube.glsl")

void fetchShading(int id, out Triangle triangle, out Shading shading) {
	triangle = triangles[id];
	shading = shadings[id];
}

bool intersectsTriangle(Ray ray, inout HitInfo hit) {
	bool aHit = false;
	HitInfo local_hit;
	local_hit.t = hit.t;
	for (int i = 0; i < numTriangles; i++) {
		Triangle triangle = triangles[i];
		bool intersects = intersectTriangle(ray, triangle, local_hit);
		if (intersects) {
			aHit = true;
			if (local_hit.t < hit.t) {
				hit = local_hit;
			}
		}
	}

	return aHit;
}

bool isLeaf(BVHNode node) {
	return node.isLeaf == 1;
}

bool hasChild(BVHNode node, int id) {
	return node.child[id] != -1;
}

BVHNode getNode(int id) {
	if (id >= 0 && id < numNodes) return bvh[id];

	BVHNode nullNode;
	nullNode.box.min = nullNode.box.max = vec3(0);
	nullNode.child[LEFT] = nullNode.child[RIGHT] = -1;
	nullNode.isLeaf = 1;
	nullNode.id = -1;
	nullNode.size = nullNode.offset = 0;
	return nullNode;
}

bool intersect(Ray ray, int offset, int size, out HitInfo hit, bool anyHit) {
	hit.t = ray.tMax;

	for (int i = 0; i < size; i++) {
		int tid = index[offset + i];
		float t, u, v, w;
		Triangle tri = triangles[tid];

		int flip = 1;
		if (!cullBackFace && !isFrontFacing(tri, ray.origin)) {
			swap(tri.a, tri.c);
			flip = -1;
		}
		atomicCounterIncrement(test_count);
		if (triangleRayIntersect(ray, tri, t, u, v, w)) {
			if (anyHit) return true;
			if (t < hit.t) {
				hit.t = t;
				hit.shape = TRIANGLE;
				hit.id = tid;
				hit.extras = vec4(u, v, w, flip);
			}
		}
	}
	return hit.t != ray.tMax;
}

bool negativeDir(Ray ray, int axis) {
	return (1 / ray.direction[axis]) < 0;
}


bool intersectsTriangle(Ray ray, out HitInfo hit, int rootIdx, bool anyHit) {
	if (numTriangles == 0) return false;
	HitInfo lHit;

	bool aHit = false;
	int toVisitOffset = 0, currentNodeIndex = rootIdx;
	int nodesToVisit[64];

	while (!aHit) {
		BVHNode node = getNode(currentNodeIndex);
		atomicCounterIncrement(test_count);
		if (intersectCube(ray, node.box, lHit)) {
			if (isLeaf(node)) {
				//hit = lHit;
				//return true;
				if (intersect(ray, node.offset, node.size, lHit, anyHit)) {
					if (anyHit) return true;
					aHit = true;
				}

				if (toVisitOffset == 0) break;
				currentNodeIndex = nodesToVisit[--toVisitOffset];
			}
			else {
				if (negativeDir(ray, node.splitAxis)) {
					nodesToVisit[toVisitOffset++] = node.child[LEFT];
					currentNodeIndex = node.child[RIGHT];
				}
				else {
					nodesToVisit[toVisitOffset++] = node.child[RIGHT];
					currentNodeIndex = node.child[LEFT];
				}
			}
		}
		else {
			if (toVisitOffset == 0) break;
			currentNodeIndex = nodesToVisit[--toVisitOffset];
		}
	}
	hit = lHit;
	return aHit;
}

bool intersectsTriangle(Ray ray, out HitInfo hit, int rootIdx) {
	return intersectsTriangle(ray, hit, rootIdx, false);
}

bool intersectsTriangleAnyHit(Ray ray, out HitInfo hit, int rootIdx) {
	return intersectsTriangle(ray, hit, rootIdx, true);
}