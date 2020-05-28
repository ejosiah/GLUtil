#version 450 core
#pragma debug(on)
#pragma optimize(off)

#pragma include("constants.glsl")
#pragma include("LightFieldProbe.glsl")

in VERTEX{
	smooth vec3 position;
	smooth vec4 lightSpacePos;
	smooth vec4 color;
	smooth vec3 normal;
	smooth vec3 tangent;
	smooth vec3 bitangent;
	smooth vec2 uv;
} vert_in;

out vec4 fragColor;

uniform vec3 camPos;
uniform bool lfp_on = false;

void main(){
	vec3 worldPos = vert_in.position;
	vec3 N = normalize(vert_in.normal);
	N = gl_FrontFacing ? N : -N;;
	vec3 V = normalize(camPos - worldPos);

	vec3 color = vec3(1);

//	if(lfp_on){
		vec3 wi = normalize(reflect(-V, N));
		vec2 texCoord = octEncode(wi);

	//	color = texture(lightFieldSurface.radianceProbeGrid.sampler, vec3(texCoord, 0)).rgb;
		color = computeGlossyRay(worldPos, V, N);
//	}

	fragColor = vec4(color, 1);
}
