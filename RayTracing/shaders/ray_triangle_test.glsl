#pragma include("ray_tracing_model.glsl")

const int ROOT = 0;
const int LEFT = 0;
const int RIGHT = 1;

struct BVHNode {
	Box box;
	int splitAxis;
	int id;
	int offset;
	int size;
	int isLeaf;
	int child[2];
};

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

bool intersect(Ray ray, int offset, int size, out HitInfo hit) {
	hit.t = ray.tMax;

	for (int i = 0; i < size; i++) {
		int tid = index[offset + i];
		float t, u, v, w;
		Triangle tri;
		fetchTriangle(tid, tri);
		if (triangleRayIntersect(ray, tri, t, u, v, w)) {
			if (t < hit.t) {
				hit.t = t;
				hit.shape = TRIANGLE;
				hit.id = tid;
				hit.extras = vec4(u, v, w, 0);
			}
		}
	}
	return hit.t != ray.tMax;
}

bool negativeDir(Ray ray, int axis) {
	return (1 / ray.direction.xyz[axis]) < 0;
}


bool intersectsTriangle(Ray ray, out HitInfo hit, int rootIdx) {
	if (numTriangles == 0) return false;
	HitInfo lHit;

	bool aHit = false;
	int toVisitOffset = 0, currentNodeIndex = rootIdx;
	int nodesToVisit[64];

	while (!aHit) {
		BVHNode node = getNode(currentNodeIndex);
		if (intersectCube(ray, node.box, lHit)) {
			if (isLeaf(node)) {
				if (intersect(ray, node.offset, node.size, lHit)) aHit = true;

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

bool triangleRayIntersect(Ray ray, Triangle tri, out float t, out float u, out float v, out float w) {
	vec3 ba = tri.b - tri.a;
	vec3 ca = tri.c - tri.a;
	vec3 pa = r.origin.xyz - tri.a;
	vec3 pq = -ray.direction.xyz;

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