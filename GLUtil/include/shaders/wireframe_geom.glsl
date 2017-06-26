noperspective out vec3 edgeDistance;
uniform mat4 viewport;

void calculateWireframe(out float ha, out float hb, out float hc){
	vec3 p0 = (viewport * (gl_in[0].gl_Position / gl_in[0].gl_Position.w)).xyz;
	vec3 p1 = (viewport * (gl_in[1].gl_Position / gl_in[1].gl_Position.w)).xyz;
	vec3 p2 = (viewport * (gl_in[2].gl_Position / gl_in[2].gl_Position.w)).xyz;

	// find the altitudes (ha, hb, hc)
	float a = distance(p1, p2);
	float b = distance(p2, p0);
	float c = distance(p1, p0);
	float alpha = acos((b*b + c*c - a*a) / (2.0*b*c));
	float beta = acos((a*a + c*c - b*b) / (2.0*a*c));
	ha = abs( c * sin(beta));
	hb = abs( c * sin(alpha));
	hc = abs(b * sin(alpha));

}