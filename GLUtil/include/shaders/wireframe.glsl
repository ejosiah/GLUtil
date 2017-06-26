struct LineInfo{
	float width;
	vec4 color;
};
uniform LineInfo line;
uniform bool wireframe;
noperspective in vec3 edgeDistance;

float getLineMixColor(){
	float d = min( min(edgeDistance.x, edgeDistance.y), edgeDistance.z);
	return smoothstep(line.width - 1, line.width + 1, d);
}

