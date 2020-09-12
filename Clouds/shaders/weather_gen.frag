#version 450 core 

#define CLOUD_COVERAGE 1
#define CLOUD_TYPE 2
#define PRECIPITATION 3
#define ALL 4

layout(binding = 0) uniform sampler2D image;
uniform int mode = 1;

in ncl_PerVertex{
	smooth vec2 texCoord;
};

out vec4 fragColor;

vec3 getColor(){
	vec3 color = texture(image, texCoord).rgb;
	switch(mode){
	case CLOUD_COVERAGE:
		return vec3(color.r, 0, 0);
	case CLOUD_TYPE:
		return vec3(0, 0, color.b);
	case PRECIPITATION:
		return vec3(0, color.g, 0);
	default:
		return color;
	}

}

void main(){
	fragColor.a = 1;
	fragColor.rgb = getColor();
}