#version 450 core
#pragma debug(on)
#pragma optimize(off)

layout( triangles ) in;
layout( triangle_strip, max_vertices = 3) out;

const int MAX_LIGHT_SOURCES = 10;

in VERTEX {
	smooth vec3 position;
	smooth vec3 normal;
	smooth vec2 texCoord;
	smooth vec4 color;
	smooth vec3 eyes;
	smooth vec4 lightDirection[MAX_LIGHT_SOURCES];
} in_vertex[];

out VERTEX {
	smooth vec3 position;
	smooth vec3 normal;
	smooth vec2 texCoord;
	smooth vec4 color;
	smooth vec3 eyes;
	smooth vec4 lightDirection[MAX_LIGHT_SOURCES];
} out_vertex;

noperspective out vec3 edgeDistance;
uniform mat4 viewport;

void main(){
	vec3 p0 = (viewport * (gl_in[0].gl_Position / gl_in[0].gl_Position.w)).xyz;
	vec3 p1 = (viewport * (gl_in[1].gl_Position / gl_in[1].gl_Position.w)).xyz;
	vec3 p2 = (viewport * (gl_in[2].gl_Position / gl_in[2].gl_Position.w)).xyz;

	// find the altitudes (ha, hb, hc)
	float a = length(p1 - p2);
	float b = length(p2 - p0);
	float c = length(p1 - p0);
	float alpha = acos((b*b + c*c - a*a) / (2.0*b*c));
	float beta = acos((a*a + c*c - b*b) / (2.0*a*c));
	float ha = abs( c * sin(beta));
	float hb = abs( c * sin(alpha));
	float hc = abs(b * sin(alpha));

	// send the triangle along with the edge distances
	edgeDistance = vec3(ha, 0, 0);
	out_vertex.position = in_vertex[0].position;
	out_vertex.normal = in_vertex[0].normal;
	out_vertex.texCoord = in_vertex[0].texCoord;
	out_vertex.eyes = in_vertex[0].eyes;
	out_vertex.color = in_vertex[0].color;
	

	for(int i = 0; i < in_vertex[0].lightDirection.length(); i++){
		out_vertex.lightDirection[i] = in_vertex[0].lightDirection[i];
	}

	gl_Position = gl_in[0].gl_Position;
	EmitVertex();

	edgeDistance = vec3(0, hb, 0);
	out_vertex.position = in_vertex[1].position;
	out_vertex.normal = in_vertex[1].normal;
	out_vertex.texCoord = in_vertex[1].texCoord;
	out_vertex.color = in_vertex[1].color;
	out_vertex.eyes = in_vertex[1].eyes;
	for(int i = 0; i < in_vertex[1].lightDirection.length(); i++){
		out_vertex.lightDirection[i] = in_vertex[1].lightDirection[i];
	}
	gl_Position = gl_in[1].gl_Position;
	EmitVertex();

	edgeDistance = vec3(0, 0, hc);
	out_vertex.position = in_vertex[2].position;
	out_vertex.normal = in_vertex[2].normal;
	out_vertex.texCoord = in_vertex[2].texCoord;
	out_vertex.color = in_vertex[2].color;
	out_vertex.eyes = in_vertex[2].eyes;
	for(int i = 0; i < in_vertex[2].lightDirection.length(); i++){
		out_vertex.lightDirection[i] = in_vertex[2].lightDirection[i];
	}
	gl_Position = gl_in[2].gl_Position;
	EmitVertex();
}