#version 450 core 

layout(binding = 0) uniform sampler2D color_buffer;
layout(binding = 1) uniform sampler2D depth_buffer;

in ncl_PerVertex{
	smooth vec2 texCoord;
};

#define SIGMA 10.0
#define BSIGMA 0.1
#define MSIZE 15

uniform vec2 resolution;

float normpdf(in float x, in float sigma)
{
	return 0.39894*exp(-0.5*x*x/(sigma*sigma))/sigma;
}

float normpdf3(in vec3 v, in float sigma)
{
	return 0.39894*exp(-0.5*dot(v,v)/(sigma*sigma))/sigma;
}

vec4 bilateral_filter(vec3 c, float d){
			//declare stuff
		const int kSize = (MSIZE-1)/2;
		float kernel[MSIZE];
		vec3 final_colour = vec3(0.0);
		
		//create the 1-D kernel
		float Z = 0.0;
		for (int j = 0; j <= kSize; ++j)
		{
			kernel[kSize+j] = kernel[kSize-j] = normpdf(float(j), SIGMA);
		}
		
		
		vec3 cc;
		float dd;
		float factor;
		float bZ = 1.0/normpdf(0.0, BSIGMA);
		//read out the texels
		for (int i=-kSize; i <= kSize; ++i)
		{
			for (int j=-kSize; j <= kSize; ++j)
			{
				cc = texture(color_buffer, (gl_FragCoord.xy+vec2(float(i),float(j))) / resolution).rgb;
				dd = texture(depth_buffer, (gl_FragCoord.xy+vec2(float(i),float(j))) / resolution).r;
				factor = normpdf(dd-d, BSIGMA)*bZ*kernel[kSize+j]*kernel[kSize+i];
				Z += factor;
				final_colour += factor*cc;

			}
		}
		
		
		return vec4(final_colour/Z, 1.0);
}

out vec4 fragColor;

void main(){
	fragColor.a = 1;
	gl_FragDepth = texture(depth_buffer, texCoord).r;
	vec3 color = texture(color_buffer, texCoord).rgb;
	float depth = gl_FragDepth;
//	fragColor = bilateral_filter(color, depth);
	fragColor.rgb = texture(color_buffer, texCoord).rgb;
}