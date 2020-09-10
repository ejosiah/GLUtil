#version 450 core
#pragma debug(on)
#pragma optimize(off)

layout(binding=10) uniform sampler2D glyph;
uniform vec4 color;
uniform vec4 backgroundColor;
uniform bool useBackgroundColor;

in vec2 texCoord;
out vec4 fragColor;

void main(){
	float alpha = texture(glyph, texCoord).r;
	fragColor = vec4(color.rgb, alpha);
}
