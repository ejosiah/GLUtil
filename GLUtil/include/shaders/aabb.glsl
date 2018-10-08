struct AABB{
    vec3 min;
    vec3 max;
};

vec3 closestPoint(in vec3 p, in AABB aabb){
    vec3 q;
    for(int i = 0; i < 3; i++){
        float v = p[i];
        if(v < aabb.min[i]) v = aabb.min[i];
        if(v > aabb.max[i]) v = aabb.max[i];
        q[i] = v;
    }

    return q;
}

float sqDistance(in vec3 p, in AABB aabb){
    float d = 0;
    for(int i = 0; i < 3; i++){
        float v = p[i];
        if(v < aabb.min[i]) d +=  (aabb.min[i] - v) * (aabb.min[i] - v);
        if(v > aabb.max[i]) d += (aabb.max[i] - v) * (aabb.max[i] - v);
    }
    return d;
}

bool pointInAABB(vec3 p, AABB aabb){
    vec3 q = closestPoint(p, aabb);
    return all(equal(p, q));
}