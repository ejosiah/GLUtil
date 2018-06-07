vec3 calculateNormal(){
    vec3 p0 = gl_in[0].gl_Position.xyz;
    vec3 p1 = gl_in[1].gl_Position.xyz;
    vec3 p2 = gl_in[2].gl_Position.xyz;

    vec3 u = p1 - p0;
    vec3 v = p2 - p0;

    return normalize(cross(u, v));
}
