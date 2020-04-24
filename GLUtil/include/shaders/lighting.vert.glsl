#pragma include("constants.glsl")
#pragma include("lightModel.glsl")
#pragma include("vertex_out.glsl")

uniform mat3 normalMatrix;
uniform bool useObjectSpace;
mat3 OLM;

vec4 getLightDirection(vec4 pos, mat4 M, in LightSource light){
	vec4 direction = vec4(0);
	if(light.position.w == 0){	// directional light
		direction = light.position;
	}
	else{	// positional light
		vec4 lightPos = (light.position/light.position.w);
		direction = (M*light.position) - pos;
	}
	return normalize(vec4( OLM * direction.xyz, 1.0));
}

void applyLight(mat4 MV, mat4 V, vec3 position, vec3 normal, vec3 tangent, vec3 bitangent){
	vec3 n = normalize(normalMatrix * normal);
	vec3 t = normalize(normalMatrix * tangent);
	vec3 b = normalize(normalMatrix * bitangent);

	vertex_out.normal = normalize(normalMatrix * normal);
	vec4 pos = MV * vec4(position, 1);
	vertex_out.position = pos.xyz;
	
	OLM = !lightModel.useObjectSpace ? mat3(1) : mat3(t.x, b.x, n.x, t.y, b.y, n.y, t.z, b.z, n.z);

	for(int i = 0; i < numLights; i++){
		vertex_out.lightDirection[i] = getLightDirection(pos, V, light[i]);
		vertex_out.spotDirection[i] = OLM * light[i].spotDirection.xyz;
	}

	vertex_out.eyes =  OLM * (lightModel.localViewer ? normalize(-pos.xyz) : vec3(0, 0, 1));
}