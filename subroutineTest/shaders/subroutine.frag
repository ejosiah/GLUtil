#version 450 core 

in ncl_PerVertex{
	smooth vec2 texCoord;
};

subroutine vec4 colorFunc();
subroutine vec4 subtractiveFunc();


subroutine uniform colorFunc color;
subroutine uniform subtractiveFunc sub_color;

subroutine (colorFunc) vec4 red(){
	return vec4(1, 0, 0, 1);
}

subroutine (colorFunc) vec4 green(){
	return vec4(0, 1, 0, 1);
}

subroutine (colorFunc) vec4 blue(){
	return vec4(0, 0, 1, 1);
}

subroutine (subtractiveFunc) vec4 yellow(){
	return vec4(1, 1, 0, 1);
}

subroutine (subtractiveFunc) vec4 cyan(){
	return vec4(0, 1, 1, 1);
}

out vec4 fragColor;
uniform bool sub = false;

void main(){
	fragColor = sub ? sub_color() : color();
}