#version 450 core

layout(binding=0) uniform sampler2D equirectangularMap;

in vec3 localPos;
out vec4 fragColor;
const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 sampleSphere(vec3 v){
	vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main(){
    vec2 uv = sampleSphere(normalize(localPos)); 
    vec3 color = texture(equirectangularMap, uv).rgb;
    
    fragColor = vec4(color, 1.0);
}