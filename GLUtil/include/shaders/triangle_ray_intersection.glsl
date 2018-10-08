bool triangleRayIntersect(vec3 a, vec3 b, vec3 c, vec3 p, vec3 q, bool lineTest, out float u, out float v, out float w, out float t){
    vec3 ba = b - a;
    vec3 ca = c - a;
    vec3 pa = p - a;
    vec3 pq = p - q;

    vec3 n = cross(ba, ca);
    float d = dot(pq, n);

    if(d <= 0) return false; // ray is either coplainar with triangle abc or facing opposite it

    t = dot(pa, n);

    if(t < 0) return false;     // ray invariant t >= 0
    if(lineTest && t > d) return false; // line invariant 0 < t < 1

    vec3 e =  cross(pq, pa);

    v = dot(e, ca);
    if(v < 0.0f || v > d) return false;

    w = -dot(e, ba);
    if(w < 0.0f || (v + w) > d) return false;

    float ood = 1.0/d;

    t *= ood;
    v *= ood;
    w *= ood;
    u = 1 - v - w;

    return true;
}


bool triangleLineIntersect(vec3 a, vec3 b, vec3 c, vec3 p, vec3 q, out float t){
    float u;
    float v;
    float w;
    float t1;
    bool res =  triangleRayIntersect(a, b, c, p, q, true, u, v, w, t1);
    t = t1;
    return res;
}