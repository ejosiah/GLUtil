#version 450 core 

smooth in vec3 diffuseColor;
out vec4 fragColor;

void main(){
	fragColor = vec4(diffuseColor, 1.0);
}