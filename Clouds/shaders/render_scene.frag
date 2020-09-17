#version 450 core 

layout(binding = 0 ) uniform sampler2D color_buffer;
layout(binding = 1 ) uniform sampler2D depth_buffer;

in ncl_PerVertex{
	smooth vec2 texCoord;
};

out vec4 fragColor;

void main(){
	fragColor.a = 1;
	gl_FragDepth = texture(depth_buffer, texCoord).r;
	fragColor.rgb = texture(color_buffer, texCoord).rgb;
}