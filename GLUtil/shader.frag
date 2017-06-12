#version 450 core
#pragma debug(on)
#pragma optimize(off)

subroutine vec4 shaderModelType(vec4 position, vec3 normal);
subroutine uniform shaderModelType shadeModel;

layout(binding=0) uniform sampler2D image0;
layout(binding=1) uniform sampler2D image1;
layout(binding=2) uniform sampler2D image2;
layout(binding=3) uniform sampler2D image3;
layout(binding=4) uniform sampler2D image4;
layout(binding=5) uniform sampler2D image5;
layout(binding=6) uniform sampler2D image6;
layout(binding=7) uniform sampler2D image7;

smooth in vec3 interpolatedPosition;
smooth in vec3 interpolatedNormal;
smooth in vec2 interpolatedTexCoord;
smooth in vec4 interpolatedColor;
uniform vec2 texCoordScale;


out vec4 fragColor;

void main(){
	fragColor = shadeModel(vec4(0, 0, 0, 0), vec3(0, 0, 0));
}



subroutine( shaderModelType )
vec4 useVertexData( vec4 position, vec3 normal){
	return interpolatedColor;
}

subroutine( shaderModelType )
vec4 useTexture( vec4 position, vec3 normal){
	return texture(image0, interpolatedTexCoord);
}

subroutine( shaderModelType )
vec4 useLightMap(vec4 position, vec3 normal){
	vec2 lightMapTexCoord = interpolatedTexCoord/texCoordScale;

	vec4 image = texture(image0, interpolatedTexCoord);
	vec4 lightMap = texture(image1, lightMapTexCoord);

	return mix(lightMap, image, lightMap.a);
}
