#version 450 core

#pragma compile(off)


uniform int numSlices;
//uniform int slice;

layout(location = 0 ) out vec4 fragColor;

layout(std430, binding = 0 ) buffer SLICES{
    int slices[];
};

in ncl_PerVertex{
	smooth vec2 uv;
   flat int slice;
};

float remap(float x, float a, float b, float c, float d);

float worleyFbm(vec3 p, float freq);

float perlinfbm(vec3 p, float freq, int octaves);

void main(){
	float z = float(slice)/numSlices;
	vec3 pos = vec3(uv, z);

	float freq = 4.;
    
    float pfbm= mix(1., perlinfbm(pos, 4., 7), .5);
    pfbm = abs(pfbm * 2. - 1.); // billowy perlin noise
    
    vec4 col = vec4(0.);
    col.g += worleyFbm(pos, freq);
    col.b += worleyFbm(pos, freq*2.);
    col.a += worleyFbm(pos, freq*4.);
    col.r += remap(pfbm, 0., 1., col.g, 1.); // perlin-worley
  

   slices[gl_Layer] = gl_Layer;
    fragColor = col;
}

#pragma include("noise.glsl")