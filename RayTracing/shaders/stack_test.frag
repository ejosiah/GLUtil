#version 450 core 

const int PUSH = 0;
const int POP = 1;

uniform int action;
uniform int itemId = 0;

#pragma include("stack.glsl")

vec4 items[5] = {
	vec4(1, 0, 0, 1),
	vec4(0, 1, 0, 1),
	vec4(0, 0, 1, 1),
	vec4(1, 1, 0, 1),
	vec4(1, 0, 1, 1)
};

in ncl_PerVertex{
	smooth vec2 texCoord;
};

out vec4 fragColor;

void main(){
	stack stack;
	init(stack);

	push(stack, 0);
	push(stack, 1);
	push(stack, 2);
	push(stack, 3);
	push(stack, 4);

	pop(stack);
	pop(stack);
	push(stack, 0);
	push(stack, 1);

	pop(stack);
	pop(stack);
	pop(stack);
	pop(stack);
	pop(stack);
	
	if(empty(stack)){
		fragColor = vec4(1);
	}
	else{
		fragColor = items[peek(stack)];
	}
}