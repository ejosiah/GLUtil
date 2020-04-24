#version 450 core
#pragma debug(on)
#pragma optimize(off)

layout(binding = 0) uniform sampler2D image0;

in VERTEX {
	smooth vec3 position;
	smooth vec3 normal;
	smooth vec2 texCoord;
	smooth vec4 color;
} vertex;

uniform float offset = 1.0 / 300.0;

uniform float kernel[9];

out vec4 fragColor;

void main(){
 vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );

	vec3 sampleTex[9];

	for(int i = 0; i < 9; i++){
		sampleTex[i] = texture(image0, vertex.texCoord + offsets[i]).rgb;
	}

	vec3 col = vec3(0);
	for(int i = 0; i < 9; i++){
		col += sampleTex[i] * kernel[i];
	}
	fragColor = vec4(col, 1.0);
}