
bool barycenter(vec3 a, vec3 b, vec3 c, vec3 p, out float u, out float v, out float w){
    vec3 v0 = b - a;
    vec3 v1 = c - a;
    vec3 v2 = p - a;

    float d00 = dot(v0, v0);
    float d10 = dot(v1, v0);
    float d11 = dot(v1, v1);
    float d20 = dot(v2, v0);
    float d21 = dot(v2, v1);

    float denom = d00 * d11 - d10 * d10;

    if (a == 0.0f) {	// use close enough test
        return  false;  // degenerate triangle
    }

    v = (d20 * d11 - d10 * d21) / denom;
    w = (d00 * d21 - d20 * d10) / denom;
    u = 1 - v - w;

    return v >= 0 && w >= 0 && (v + w) <= 1;
}