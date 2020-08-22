#version 450 core

#pragma program("slicer")
#pragma compile(off)

int FindAbsMax(vec3 v) {
	v = abs(v);
	int max_dim = 0;
	float val = v.x;
	if (v.y > val) {
		val = v.y;
		max_dim = 1;
	}
	if (v.z > val) {
		val = v.z;
		max_dim = 2;
	}
	return max_dim;
}