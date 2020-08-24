#version 450 core

in ncl_PerVertex{
	smooth vec3 sunDir;
	smooth vec3 norm;
};

layout(location = 0) out vec4 fragColor;

void main(){
	vec3 S = normalize(sunDir);
	vec3 N = normalize(norm);
	float sun = dot(-S, N); 
	float sun_pos = smoothstep(0.9995, 0.99995, sun);
	vec3 sun_color = vec3(1, 0.55, 0.15) * 1000;

	vec3 deep_blue = vec3(0.3421052632, 0.9029850746, 2.311688312);
	vec3 light_blue = deep_blue + 1;

	/* Lighter around the horizon */
	vec3 sky_color = mix(deep_blue, light_blue, smoothstep(-1.0, 0.7, dot(normalize(N), vec3(0.0, 1.0, 0.0))));
	/* Lighter around the sun */
	sky_color += 0.15 * mix(vec3(0.0), vec3(1.0), smoothstep(0.85, 1.0, sun));
	sky_color += 0.2 * mix(vec3(0.0), vec3(1.0), smoothstep(0.75, 1.0, sun));

	vec3 color = sky_color + sun_pos * sun_color;
	fragColor = vec4(color, 1.0);
	//fragColor = vec4(0);
}