#version 450 core 

layout(binding = 0 ) uniform sampler2D color_buffer;
layout(binding = 1 ) uniform sampler2D depth_buffer;

in ncl_PerVertex{
	smooth vec2 texCoord;
};

out vec4 fragColor;

void main(){
	gl_FragDepth = texture(depth_buffer, texCoord).r;
	fragColor = texture(color_buffer, texCoord);
}