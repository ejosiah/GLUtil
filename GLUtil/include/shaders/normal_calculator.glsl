vec3 calculateNormal(){
    vec3 p0 = gl_in[0].gl_Position;
    vec3 p1 = gl_in[1].gl_Position;
    vec3 p2 = gl_in[2].gl_Position;

    vec3 u = distance(p0, p1);
    vec3 v = distance(p2, p1);

    return cross(u, v);
}
