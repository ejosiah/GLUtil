#version 450 core

layout(triangles, invocations = 2) in;
layout(triangle_strip, max_vertices=128) out;


in ncl_PerVertex{
	smooth vec2 texCoord;
} ncl_in[3];

out ncl_PerVertex{
	smooth vec2 texCoord;
	flat int layer;
};

uniform int numLayers;
uniform int columns = 8;
uniform float aspectRatio = 1;
uniform bool renderAll = true;
uniform int uLayer;

mat4 translate4x4(vec3 t) {
    return mat4(1,0,0,0,
                  0,1,0,0,
                  0,0,1,0,
                  t,1);
}

mat4 scale4x4(vec3 s){
    return mat4(s.x,0,0,0,
                  0,s.y,0,0,
                  0,0,s.z,0,
                  0, 0, 0,1);
}

const int MAX_TRIANGLES_PER_INSTANCE = 42;

void main(){
    int cols = columns;
    while(numLayers < cols) cols /= 2;
	float w = 1.0/cols;
	float h = w/aspectRatio;
	mat4 s = scale4x4(vec3(w, h, 1));

    if(renderAll){
        int level = gl_InvocationID * MAX_TRIANGLES_PER_INSTANCE;
        for(int level = gl_InvocationID * 42; level < numLayers; level++){
            gl_Layer = level;
            for(int i = 0; i < gl_in.length(); i++){
                layer = level;
                texCoord = ncl_in[i].texCoord;
                float x = w-1 + (level%cols) * w * 2;
                float y = 1 - h - ((level/cols) * (h * 2));
                mat4 model = translate4x4(vec3(x, y, 0)) *  s;
               // mat4 model = mat4(1);
                gl_Position = model * gl_in[i].gl_Position;
                EmitVertex();
            }
            EndPrimitive();
        }
    }else{
        for(int i = 0; i < gl_in.length(); i++){
            layer = uLayer;
            texCoord = ncl_in[i].texCoord;
            gl_Position = gl_in[i].gl_Position;
            EmitVertex();
        }
        EndPrimitive();
    }

}