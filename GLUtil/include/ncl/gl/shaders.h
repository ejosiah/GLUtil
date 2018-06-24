#pragma once
#include <iostream>
const std::string basic_frag_shader = "#version 450 core\n#pragma debug(on)\n#pragma optimize(off)\n\nlayout(location=0) out vec4 vFragColor; //fragment shader output\n \nsmooth in vec4 color; //interpolated colour from the vertex shader\n\nvoid main() { \n	//set the input colour from the vertex shader as fragment shader output\n	vFragColor = color;	 \n}\n";
const std::string font_frag_shader = "#version 450 core\n#pragma debug(on)\n#pragma optimize(off)\n\nlayout(binding=10) uniform sampler2D glyph;\nuniform vec4 color;\nuniform vec4 backgroundColor;\nuniform bool useBackgroundColor;\n\nin vec2 texCoord;\nout vec4 fragColor;\n\nvoid main(){\n	float alpha = texture(glyph, texCoord).r;\n	fragColor = vec4(1, 1, 1, alpha) * color;\n}\n";
const std::string font_vert_shader = "#version 450 core\n#pragma debug(on)\n#pragma optimize(off)\n\nlayout(location=0) in vec4 coord;\n\nuniform mat4 P;\nsmooth out vec2 texCoord;\n\nvoid main(){\n	gl_Position = P * vec4(coord.xy, 0, 1);\n	texCoord = coord.zw;\n}\n";
const std::string identity_frag_shader = "#version 450 core\n#pragma debug(on)\n#pragma optimize(off)\n\nin VERTEX {\n	smooth vec3 position;\n	smooth vec3 normal;\n	smooth vec2 texCoord;\n	smooth vec4 color;\n} vertex;\n\nout vec4 fragColor;\n\nvoid main(){\n	fragColor = vec4(0);\n}\n";
const std::string identity_vert_shader = "#version 450 core\n#pragma debug(on)\n#pragma optimize(off)\n\nuniform mat4 M;\nuniform mat4 V;\nuniform mat4 P;\nuniform mat4 MVP;\n\nlayout(location=0) in vec3 position;\nlayout(location=1) in vec3 normal;\nlayout(location=2) in vec3 tangent;\nlayout(location=3) in vec3 bitangent;\nlayout(location=4) in vec4 color;\nlayout(location=5) in vec2 uv;\n\nout VERTEX {\n	smooth vec3 position;\n	smooth vec3 normal;\n	smooth vec2 texCoord;\n	smooth vec4 color;\n} vertex;\n\nvoid main(){\n	mat4 MV = V * M;\n	mat3 NM = transpose(inverse(mat3(MV)));\n	vertex.normal = normalize(NM * normal);\n	vertex.position = (MV * vec4(position, 1)).xyz;\n	vertex.texCoord = uv;\n	vertex.color = color;\n	gl_Position = MVP * vec4(position, 1);\n}\n";
const std::string lightMap_frag_shader = "#version 450 core\n#pragma debug(on)\n#pragma optimize(off)\n\nlayout(binding=0) uniform sampler2D image0;\nlayout(binding=1) uniform sampler2D image1;\n\nsmooth in vec2 interpolatedTexCoord;\nsmooth in vec2 interpolatedLightTexCoord;\n\nout vec4 fragColor;\n\nvoid main(){\n	vec4 color = texture(image0, interpolatedTexCoord);\n	vec4 light = texture(image1, interpolatedLightTexCoord);\n	fragColor = light * color;\n}\n";
const std::string lightMap_vert_shader = "#version 450 core\n#pragma debug(on)\n#pragma optimize(off)\n\nuniform mat4 M;\nuniform mat4 V;\nuniform mat4 P;\n\nlayout(location=0) in vec3 position;\nlayout(location=1) in vec3 normal;\nlayout(location=2) in vec3 tangent;\nlayout(location=3) in vec3 bitangent;\nlayout(location=4) in vec4 color;\nlayout(location=5) in vec2 uv;\nlayout(location=6) in vec2 lightUV;\n\nsmooth out vec2 interpolatedTexCoord;\nsmooth out vec2 interpolatedLightTexCoord;\n\nvoid main(){\n	interpolatedTexCoord = uv;\n	interpolatedLightTexCoord = lightUV;\n	gl_Position = P * V * M * vec4(position, 1);\n}\n";
const std::string noise_frag_shader = "#version 450 core\n#pragma debug(on)\n#pragma optimize(off)\n\n#define PI 3.14159265\n\nlayout(binding=0) uniform sampler2D noise;\n\nuniform vec4 sky = vec4(0.3, 0.3, 0.9, 1.0);\nuniform vec4 cloud = vec4(1.0);\n\nin VERTEX {\n	smooth vec3 position;\n	smooth vec3 normal;\n	smooth vec2 texCoord;\n	smooth vec4 color;\n} vertex;\n\nout vec4 color;\n\nvoid main(){\n	vec4 oct = texture(noise, vertex.texCoord);\n	float sum =  ( oct.r + oct.g + oct.b + oct.a - 1)/2;\n	float t = (cos(sum * PI) + 1.0)/2.0;\n	vec4 c = mix(sky, cloud, t);\n	color = vec4(c.rgb, 1.0);\n}\n";
const std::string pass_through_frag_shader = "#version 450 core\n#pragma debug(on)\n#pragma optimize(off)\n\nsmooth in vec4 fColor;\nout vec4 fragColor;\n\nvoid main(){\n	fragColor = vec4(1);\n}\n";
const std::string pass_through_geom_shader = "#version 450 core\n#pragma debug(on)\n#pragma optimize(off)\n\nlayout(triangles) in;\nlayout(triangle_strip, max_vertices = 3) out;\n\nvoid main(){\n	for(int i = 0; i < gl_in.length(); i++){\n		gl_Position = gl_in[i].gl_Position;\n		EmitVertex();\n	}\n	EmitPrimitive();\n}\n";
const std::string pass_through_tcs_shader = "#version 450 core\n#pragma debug(on)\n#pragma optimize(off)\n\nlayout (vertices = 16) out;\n\nvoid main(){\n	\n	gl_TessLevelOuter[0] = 4;\n	gl_TessLevelOuter[1] = 4;\n	gl_TessLevelOuter[2] = 4;\n	gl_TessLevelOuter[3] = 4;\n\n	gl_TessLevelInner[0] = 10;\n	gl_TessLevelInner[1] = 10;\n\n	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;\n}\n";
const std::string pass_through_tes_shader = "#version 450 core\n#pragma debug(on)\n#pragma optimize(off)\n\nlayout (quads, equal_spacing, ccw) in;\n\nvoid main(){\n	gl_Position = gl_in[gl_InvocationID].gl_Position;\n}\n";
const std::string pass_through_vert_shader = "#version 450 core\n#pragma debug(on)\n#pragma optimize(off)\n\nuniform mat4 M;\nuniform mat4 V;\nuniform mat4 P;\n\nlayout(location=0) in vec3 position;\nlayout(location=5) in vec4 color;\nsmooth out vec4 fColor;\n\nvoid main(){\n	fColor = color;\n	gl_Position = P * V * M * vec4(position, 1);\n}\n";
const std::string per_fragment_lighing_frag_shader = "#version 450 core\n#pragma debug(on)\n#pragma optimize(off)\n\nconst int MAX_LIGHT_SOURCES = 10;\nconst int MAX_TEXTURES = 8;\n\nlayout(binding = 0) uniform sampler2D image0;\nlayout(binding = 1) uniform sampler2D image1;   // normalMap if using object  space\nlayout(binding=2) uniform sampler2D image2;\nlayout(binding=3) uniform sampler2D image3;\nlayout(binding=4) uniform sampler2D image4;\nlayout(binding=5) uniform sampler2D image5;\nlayout(binding=6) uniform sampler2D image6;\nlayout(binding=7) uniform sampler2D image7;\n\nstruct LightSource{\n	vec4 position;\n	vec4 ambient;\n	vec4 diffuse;\n	vec4 specular;\n	vec4 spotDirection;\n	float spotAngle;\n	float spotExponent;\n	float kc;\n	float ki;\n	float kq;\n	bool transform;\n	bool on;\n};\n\nstruct Material{\n	vec4 emission;\n	vec4 ambient;\n	vec4 diffuse;\n	vec4 specular;\n	float shininess;\n	int diffuseMat;\n	int specularMat;\n	int ambientMat;\n	int bumpMap;\n};\n\nstruct LineInfo{\n	float width;\n	vec4 color;\n};\n\nstruct ToonShader{\n	int levels;\n	float scaleFactor;\n};\n\nstruct LightModel {\n	bool localViewer;\n	bool twoSided;\n	bool useObjectSpace;\n	bool celShading;\n	vec4 globalAmbience;\n	bool colorMaterial;\n};\n\nuniform mat4 MV;\nuniform mat4 V;\nuniform LightSource light[MAX_LIGHT_SOURCES];\nuniform Material material[2];\nuniform LineInfo line;\nuniform bool wireframe;\nuniform LightModel lightModel;\nuniform bool activeTextures[MAX_TEXTURES];\nuniform int blendTex[MAX_TEXTURES - 1];\n\nToonShader toonShader;\n\nin VERTEX{\n	smooth vec3 position;\n	smooth vec3 normal;\n	smooth vec2 texCoord;\n	smooth vec4 color;\n	smooth vec3 eyes;\n	smooth vec4 lightDirection[MAX_LIGHT_SOURCES];\n} vertex;\nnoperspective in vec3 edgeDistance;\n\nout vec4 fragColor;\n\nvec4 getAmbience(Material m){\n	return lightModel.colorMaterial ? vertex.color : m.ambient;\n}\n\nvec4 getDiffuse(Material m){\n	return lightModel.colorMaterial ? vertex.color : m.diffuse;\n}\n\nfloat daf(float dist, LightSource light){\n	return 1.0 / (light.kc + light.ki * dist + light.kq * dist * dist);\n}\n\nfloat saf(LightSource light, vec3 lightDirection){\n	vec3 l = normalize(lightDirection);\n	vec3 d =   normalize(mat3(V) * light.spotDirection.xyz);\n	float h = light.spotExponent;\n	\n	if(light.spotAngle >= 180) 	return 1.0;\n	\n	float _LdotD = dot(-l, d);\n	float cos_spotAngle = cos(radians(light.spotAngle));\n	\n	if(_LdotD < cos_spotAngle) return 0.0;\n	\n	return pow(_LdotD, h); \n}\n\nfloat getLineMixColor(){\n	float d = min( min(edgeDistance.x, edgeDistance.y), edgeDistance.z);\n	return smoothstep(line.width - 1, line.width + 1, d);\n}\n\nvec4 apply(LightSource light, vec4 direction, Material m){\n	if(!light.on) return vec4(0);\n	toonShader.levels = 3;\n	toonShader.scaleFactor = 1.0 / toonShader.levels;\n	vec3 n = gl_FrontFacing ? normalize(vertex.normal) : normalize(-vertex.normal);\n	n = lightModel.useObjectSpace ? (2.0 * texture(image1, vertex.texCoord) - 1.0).xyz : n;\n//	n = (2.0 * texture(image1, vertex.texCoord) - 1.0).xyz;\n	vec3 l = normalize(direction.xyz);\n	float f = m.shininess;\n		\n	float _daf = daf(length(l), light);\n\n	float _saf = saf(light, l);\n	\n	vec4 ambient = light.ambient * m.ambient;\n	\n	vec4 diffuse =  lightModel.celShading ? floor(max(dot(l, n), 0) * toonShader.levels) * toonShader.scaleFactor * light.diffuse *  getDiffuse(m)\n							: max(dot(l, n), 0)  * light.diffuse * m.diffuse;\n	\n	vec3 e = normalize(vertex.eyes);\n	vec3 s = normalize(l + e);	// half way vector between light direction and eyes\n	vec4 specular = pow(max(dot(s, n), 0), f) * light.specular * m.specular;\n\n	return  _daf * _saf * ((ambient + diffuse) + specular); \n}\n\nvec4 phongLightModel(){\n	Material m = !lightModel.twoSided ?  material[0] : gl_FrontFacing ? material[0] : material[1];\n	vec4 color = m.emission + lightModel.globalAmbience * getAmbience(m);\n\n	for(int i = 0; i < light.length(); i++ ) \n		color += apply(light[i], vertex.lightDirection[i], m);\n\n	return color;\n}\n\nvec4 texColor(){\n	vec4 colors[] = {\n		texture(image0, vertex.texCoord), texture(image1, vertex.texCoord)\n		, texture(image2, vertex.texCoord), texture(image3, vertex.texCoord)\n		, texture(image4, vertex.texCoord), texture(image5, vertex.texCoord)\n		, texture(image6, vertex.texCoord), texture(image7, vertex.texCoord)\n	};	// TODO use texCoord array\n\n	vec4 color = vec4(1);\n	int next = 0;\n	for(; next < MAX_TEXTURES; next++){\n		if(activeTextures[next]){\n			color = colors[next];\n			break;\n		}\n	}\n//	for(int i = 0; i < blendTex.length(); i++){\n//		int blendId = blendTex[i];\n//		if(blendId != 0){\n//			vec4 blendColor = colors[blendId];\n//			color = mix(color, blendColor, blendColor.a);\n//		}\n//	}\n	return color;\n}\n\nvoid main(){\n	fragColor = phongLightModel() * texColor();\n	fragColor = wireframe ? mix(line.color, fragColor, getLineMixColor()) : fragColor;\n//	fragColor = texture(image0, vertex.texCoord);\n}\n";
const std::string per_fragment_lighing_vert_shader = "#version 450 core\n#pragma debug(on)\n#pragma optimize(off)\n\nconst int MAX_LIGHT_SOURCES = 10;\n\nlayout(location=0) in vec3 position;\nlayout(location=1) in vec3 normal;\nlayout(location=2) in vec3 tangent;\nlayout(location=3) in vec3 bitangent;\nlayout(location=4) in vec4 color;\nlayout(location=5) in vec2 uv;\n\nuniform struct LightSource{\n	vec4 position;\n	vec4 ambient;\n	vec4 diffuse;\n	vec4 specular;\n	vec4 spotDirection;\n	float spotAngle;\n	float spotExponent;\n	float kc;\n	float ki;\n	float kq;\n	bool transform;\n	bool on;\n} light[MAX_LIGHT_SOURCES];\n\nstruct LightModel {\n	bool localViewer;\n	bool twoSided;\n	bool useObjectSpace;\n	bool celShading;\n	vec4 globalAmbience;\n	bool colorMaterial;\n};\n\n\nout VERTEX {\n	smooth vec3 position;\n	smooth vec3 normal;\n	smooth vec2 texCoord;\n	smooth vec4 color;\n	smooth vec3 eyes;\n	smooth vec4 lightDirection[MAX_LIGHT_SOURCES];\n} vertex;\n\nnoperspective out vec3 edgeDistance;\nuniform mat4 V;\nuniform mat4 M;\nuniform mat4 P;\nuniform LightModel lightModel;\nuniform mat3 normalMatrix;\nuniform bool useObjectSpace;\n\nmat3 OLM;\nmat4 MV;\nmat4 MVP;\nmat3 NM;\n\nvec4 getLightDirection(vec4 pos, in LightSource light){\n	vec4 direction = vec4(0);\n	if(light.position.w == 0){	// directional light\n		if(light.transform){\n			direction = V * light.position;\n		}\n		direction = light.position;\n	}\n	else{	// positional light\n		vec4 lightPos = (light.position/light.position.w);\n		if(light.transform){\n			direction = (V*light.position) - pos;\n		}else{\n			direction = light.position - pos;\n		}\n	}\n	return normalize(vec4( OLM * direction.xyz, 1.0));\n}\n\nvoid main(){\n	vec3 n = normalize(normalMatrix * normal);\n	vec3 t = normalize(normalMatrix * tangent);\n	vec3 b = normalize(normalMatrix * bitangent);\n\n	MV = V * M;\n	MVP = P * MV;\n\n	vertex.normal =  n;\n	vec4 pos = MV * vec4(position, 1);\n	vertex.position = pos.xyz;\n	\n	OLM = !lightModel.useObjectSpace ? mat3(1) : mat3(t.x, b.x, n.x, t.y, b.y, n.y, t.z, b.z, n.z);\n\n	for(int i = 0; i < light.length(); i++){\n		vertex.lightDirection[i] = getLightDirection(pos, light[i]);\n	}\n\n	vertex.eyes =  OLM * (lightModel.localViewer ? normalize(-pos.xyz) : vec3(0, 0, 1));\n\n	vertex.texCoord = uv;\n	vertex.color = color;\n	gl_Position = MVP * vec4(position, 1);\n}\n";
const std::string phong_lighting_vert_shader = "#version 450 core\n#pragma debug(on)\n#pragma optimize(off)\n\nstruct LightSource{\n	vec4 position;\n	vec4 ambient;\n	vec4 diffuse;\n	vec4 specular;\n	vec4 spotDirection;\n	float spotAngle;\n	float spotExponent;\n	float kc;\n	float ki;\n	float kq;\n	bool transform;\n};\n\nstruct Material{\n	vec4 emission;\n	vec4 ambient;\n	vec4 diffuse;\n	vec4 specular;\n	float shininess;\n};\n\nlayout(location=0) in vec3 vVertex;		//per-vertex position\nlayout(location=1) in vec3 vNormal;		//per-vertex normal\n \n//uniforms  \nuniform mat4 MVP;				//combined modelview projection matrix\nuniform mat4 MV;				//modelview matrix\nuniform mat3 normalMatrix;					//normal matrix\nuniform LightSource light0;\nuniform Material material;\n\n//shader outputs to the fragment shader\nsmooth out vec4 color;    //final diffuse colour to the fragment shader\n\n//shader constant\nconst vec3 vEyeSpaceCameraPosition = vec3(0,0,0); //eye is at vec3(0,0,0) in eye space\n\nvoid main()\n{ 	\n\n	vec3 light_position = light0.position.xyz;\n	vec3 diffuse_color = material.diffuse.xyz;\n	vec3 specular_color = material.specular.xyz;\n	float shininess = material.shininess;\n	mat3 N = normalMatrix;\n\n	//multiply the object space light position with the modelview matrix \n	//to get the eye space light position\n	vec4 vEyeSpaceLightPosition = MV*vec4(light_position,1);\n\n	//multiply the object space vertex position with the modelview matrix \n	//to get the eye space vertex position\n	vec4 vEyeSpacePosition = MV*vec4(vVertex,1); \n\n	//multiply the object space normal with the normal matrix \n	//to get the eye space normal\n	vec3 vEyeSpaceNormal   = normalize(N*vNormal);\n\n	//get the light vector\n	vec3 L = normalize(vEyeSpaceLightPosition.xyz-vEyeSpacePosition.xyz);\n	//get the view vector\n	vec3 V = normalize(vEyeSpaceCameraPosition.xyz-vEyeSpacePosition.xyz);\n	//get the half way vector between light and view vectors\n	vec3 H = normalize(L+V);\n\n	//calculate the diffuse and specular components\n	float diffuse = max(0, dot(vEyeSpaceNormal, L));\n	float specular = max(0, pow(dot(vEyeSpaceNormal, H), shininess));\n\n	//calculate the final colour by adding the diffuse and specular components\n	color = diffuse*vec4(diffuse_color,1) + specular*vec4(specular_color, 1);\n\n	//multiply the combiend modelview projection matrix with the object space vertex\n	//position to get the clip space position\n    gl_Position = MVP*vec4(vVertex,1); \n}\n \n";
const std::string phong_lighting2_vert_shader = "\n#version 450 core\n#pragma debug(on)\n#pragma optimize(off)\n\nstruct LightSource{\n	vec4 position;\n	vec4 ambient;\n	vec4 diffuse;\n	vec4 specular;\n	vec4 spotDirection;\n	float spotAngle;\n	float spotExponent;\n	float kc;\n	float ki;\n	float kq;\n	bool transform;\n};\n\nstruct Material{\n	vec4 emission;\n	vec4 ambient;\n	vec4 diffuse;\n	vec4 specular;\n	float shininess;\n};\n\nuniform mat4 MV;\nuniform mat4 MVP;\nuniform mat3 normalMatrix;\n\nuniform LightSource light0;\nuniform Material material;\nuniform vec4 globalAmbience;\nuniform bool localViewer;\nuniform bool eyesAtCamera;\n\nlayout(location=0) in vec3 position;\nlayout(location=1) in vec3 normal;\nlayout(location=2) in vec3 tangent;\nlayout(location=3) in vec3 bitangent;\nlayout(location=4) in vec4 color;\nlayout(location=5) in vec2 uv;\n\nout VERTEX {\n	smooth vec3 position;\n	smooth vec3 normal;\n	smooth vec2 texCoord;\n	smooth vec4 color;\n} vertex;\n\nvec3 lightDirection(vec4 pos, LightSource light);\n\nfloat daf(float dist, LightSource light);\n\nfloat saf(LightSource light, vec3 lightDirection);\n\n\nvoid main(){\n	Material m = material;\n	vec3 n = normalize(normalMatrix * normal);\n	vec4 pos = (MV * vec4(position, 1));\n//	vec3 l = lightDirection(pos, light0);\n	vec3 l = normalize((MV*light0.position).xyz - pos.xyz);\n	float f = m.shininess;\n	\n	vec4 gobAmbient = m.emission + globalAmbience * m.ambient;\n	\n	float _daf = daf(length(l), light0);\n	float _saf = saf(light0, l);\n	\n	vec4 ambient = light0.ambient * m.ambient;\n	\n	vec4 diffuse = max(dot(l, n), 0) * light0.diffuse * m.diffuse;\n	\n	vec3 e = normalize(-pos.xyz);\n	vec3 s = normalize(l + e);	// half way vector between light direction and eyes\n	vec4 specular = pow(max(dot(s, n), 0), f) * light0.specular * m.specular;\n\n\n	vertex.color = gobAmbient + _daf * _saf * ((ambient + diffuse) + specular); \n\n	gl_Position = MVP * vec4(position, 1);\n}\n\n\nfloat daf(float dist, LightSource light){\n	return 1.0 / (light.kc + light.ki * dist + light.kq * dist * dist);\n}\n\nfloat saf(LightSource light, vec3 lightDirection){\n	vec3 l = normalize(lightDirection);\n	vec3 d = normalize(light.spotDirection.xyz);\n	float h = light.spotExponent;\n	\n	if(light.spotAngle >= 180) 	return 1.0;\n	\n	float _LdotD = dot(-l, d);\n	float cos_spotAngle = cos(radians(light.spotAngle));\n	\n	if(_LdotD < cos_spotAngle) return 0.0;\n	\n	return pow(_LdotD, h); \n}\n";
const std::string quad_tes_shader = "#version 450 core\n#pragma debug(on)\n#pragma optimize(off)\n\nlayout (quads, equal_spacing, ccw) in;\n\nuniform mat4 MVP;\n\nout VERTEX {\n	smooth vec3 position;\n	smooth vec3 normal;\n	smooth vec2 texCoord;\n	smooth vec4 color;\n} vertex;\n\nsmooth in vec4 vcolor[];\n\nvec3 calculateNormal(){\n	vec3 p0 = gl_in[0].gl_Position.xyz;\n	vec3 p1 = gl_in[2].gl_Position.xyz;\n	vec3 p2 = gl_in[1].gl_Position.xyz;\n\n	vec3 a = p0 - p1;\n	vec3 b = p2 - p1;\n	\n	return cross(a, b); \n}\n\nvoid main(){\n	float u = gl_TessCoord.x;\n	float v = gl_TessCoord.y;\n	float i_u = 1 - u;\n	float i_v = 1 - v;\n\n	vec4 p0 = gl_in[0].gl_Position;\n	vec4 p1 = gl_in[1].gl_Position;\n	vec4 p2 = gl_in[2].gl_Position;\n	vec4 p3 = gl_in[3].gl_Position;\n\n	vec4 p = p0 * i_u * i_v + \n			 p1 * u * i_v + \n			 p3 * v * i_u +\n			 p2 * u * v;\n\n	vec3 pn = calculateNormal();\n\n	vertex.normal = normalize(pn + p.xyz);\n\n	vertex.texCoord = gl_TessCoord.xy;\n	vertex.color = vcolor[0];\n	gl_Position = MVP * p;\n}\n";
const std::string quad_vert_shader = "#version 450 core\n#pragma debug(on)\n#pragma optimize(off)\n\nlayout(location=0) in vec3 position;\nlayout(location=4) in vec4 color;\n\n\nlayout(binding=0) uniform samplerBuffer faces_tbo;\n\nsmooth out vec4 vcolor;\n\nmat4 model(){\n	return mat4(\n		texelFetch(faces_tbo, gl_InstanceID * 4),\n		texelFetch(faces_tbo, gl_InstanceID * 4 + 1),\n		texelFetch(faces_tbo, gl_InstanceID * 4 + 2),\n		texelFetch(faces_tbo, gl_InstanceID * 4 + 3)\n	);\n	return mat4(1);\n}\n\n\nvoid main(){\n	vcolor = color;\n	gl_Position = model() * vec4(position, 1);\n}\n";
const std::string simple_light_frag_shader = "";
const std::string teapot_frag_shader = "#version 450 core\n#pragma debug(on)\n#pragma optimize(off)\n\nin VERTEX {\n	smooth vec3 position;\n	smooth vec3 normal;\n	smooth vec2 texCoord;\n	smooth vec4 color;\n	smooth vec4 lightDirection;\n} in_vertex;\nnoperspective in vec3 edgeDistance;\n\n\nout vec4 fragColor;\n\nvoid main(){\n	fragColor = in_vertex.color;\n}\n";
const std::string teapot_tcs_shader = "#version 450 core\n#pragma debug(on)\n#pragma optimize(off)\n\nuniform int grids;\n\nlayout(vertices = 16) out;\n\nin VERTEX{\n	smooth vec4 color;\n} in_vertex[];\n\nout VERTEX{\n	smooth vec4 color;\n} out_vertex[];\n\nvoid main(){\n	gl_TessLevelOuter[0] = float(grids);\n	gl_TessLevelOuter[1] = float(grids);\n	gl_TessLevelOuter[2] = float(grids);\n	gl_TessLevelOuter[3] = float(grids);\n\n	gl_TessLevelInner[0] = float(grids);\n	gl_TessLevelInner[1] = float(grids);\n\n	out_vertex[gl_InvocationID].color = in_vertex[gl_InvocationID].color;\n\n	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;\n}\n";
const std::string teapot_tes_shader = "#version 450 core\n#pragma debug(on)\n#pragma optimize(off)\n\nlayout( quads, equal_spacing, ccw ) in;\n\nconst int MAX_LIGHT_SOURCES = 10;\n\nuniform mat4 LMV;\nuniform mat4 MV;\nuniform mat4 MVP;\nuniform mat3 normalMatrix;\n\nuniform struct LightSource{\n	vec4 position;\n	vec4 ambient;\n	vec4 diffuse;\n	vec4 specular;\n	vec4 spotDirection;\n	float spotAngle;\n	float spotExponent;\n	float kc;\n	float ki;\n	float kq;\n	bool transform;\n} light[MAX_LIGHT_SOURCES];\n\nin VERTEX{\n	smooth vec4 color;\n} in_vertex[];\n\nout VERTEX {\n	smooth vec3 position;\n	smooth vec3 normal;\n	smooth vec2 texCoord;\n	smooth vec4 color;\n	smooth vec4 lightDirection[MAX_LIGHT_SOURCES];\n} out_vertex;\nnoperspective out vec3 edgeDistance;\n\n\nconst vec4 bc = vec4(1, 3, 3, 1);\n\nfloat B(int i,  float u){\n	return bc[i] * pow(1.0 - u, 3 - i) * pow(u, i);\n}\n\nvec4 getLightDirection(vec4 pos, in LightSource light){\n	if(light.position.w == 0){	// directional light\n		if(light.transform){\n			return normalize(LMV*light.position);\n		}\n		return normalize(light.position);\n	}\n	else{	// positional light\n		vec4 lightPos = (light.position/light.position.w);\n		if(light.transform){\n			return normalize((LMV*light.position) - pos);\n		}\n		return normalize(light.position - pos);\n	}\n}\n\nfloat partialD(int i,  float u){\n	float du[] = {\n		-3.0 * (1 - u) * (1 - u),\n		 -6.0 * u * (1-u) + 3.0 * (1-u) * (1-u),\n		 -3.0 * u * u + 6.0 * u * (1-u),\n		 3.0 * u * u\n	};\n	return du[i];\n}\n\nvoid main(){\n	float u = gl_TessCoord.x;\n	float v = gl_TessCoord.y;\n\n	vec4 p = vec4(0);\n	vec4 du = vec4(0);\n	vec4 dv = vec4(0);\n	for(int i = 0; i < 4; i++){\n		for(int j = 0; j < 4; j++){\n			p += B(i, u) * B(j, v) * gl_in[i * 4 + j].gl_Position; \n			du += partialD(i, u) * B(j, v) * gl_in[i * 4 + j].gl_Position; \n			dv += B(i, u) * partialD(j, v) * gl_in[i * 4 + j].gl_Position;\n		}\n	}\n\n	vec3 normal = normalize(cross(du.xyz, dv.xyz));\n	vec4 pos = MV * p;\n	out_vertex.position = pos.xyz;\n	for(int i = 0; i < light.length(); i++){\n		out_vertex.lightDirection[i] = getLightDirection(pos, light[i]);\n	}\n	out_vertex.texCoord = gl_TessCoord.xy;\n	out_vertex.normal = normalize(normalMatrix * normal);\n	out_vertex.color = in_vertex[0].color;\n	gl_Position = MVP * p;\n\n}\n";
const std::string teapot_vert_shader = "#version 450 core\n#pragma debug(on)\n#pragma optimize(off)\n\nlayout(location=0) in vec3 position;\nlayout(location=4) in vec4 color;\n\nout VERTEX{\n	smooth vec4 color;\n} out_vertex;\n\nvoid main(){\n	out_vertex.color = color;\n	gl_Position = vec4(position, 1);\n}\n";
const std::string texture_frag_shader = "#version 450 core\n#pragma debug(on)\n#pragma optimize(off)\n\nlayout(binding = 0) uniform sampler2D image0;\nlayout(binding = 1) uniform sampler2D image1;\n\nin VERTEX {\n	smooth vec3 position;\n	smooth vec3 normal;\n	smooth vec2 texCoord;\n	smooth vec4 color;\n} vertex;\n\nout vec4 fragColor;\n\nvoid main(){\n	vec4 color = texture(image0, vertex.texCoord);\n	vec4 oldLeather = texture(image1, vertex.texCoord);\n	fragColor = mix(color, oldLeather, oldLeather.a);\n	fragColor = color;\n}\n";
const std::string ui_frag_shader = "#version 450 core\n#pragma debug(on)\n#pragma optimize(off)\n\nuniform vec4 color;\n\nout vec4 fragColor;\n\nvoid main(){\n	fragColor = color;\n}\n";
const std::string ui_vert_shader = "#version 450 core\n#pragma debug(on)\n#pragma optimize(off)\n\nuniform mat4 MVP;\n\nlayout(location=0) in vec3 position;\n\nvoid main(){\n	gl_Position = MVP * vec4(position, 1);\n}\n";
const std::string wireframe_geom_shader = "#version 450 core\n#pragma debug(on)\n#pragma optimize(off)\n\nlayout( triangles ) in;\nlayout( triangle_strip, max_vertices = 3) out;\n\nconst int MAX_LIGHT_SOURCES = 10;\n\nin VERTEX {\n	smooth vec3 position;\n	smooth vec3 normal;\n	smooth vec2 texCoord;\n	smooth vec4 color;\n	smooth vec3 eyes;\n	smooth vec4 lightDirection[MAX_LIGHT_SOURCES];\n} in_vertex[];\n\nout VERTEX {\n	smooth vec3 position;\n	smooth vec3 normal;\n	smooth vec2 texCoord;\n	smooth vec4 color;\n	smooth vec3 eyes;\n	smooth vec4 lightDirection[MAX_LIGHT_SOURCES];\n} out_vertex;\n\nnoperspective out vec3 edgeDistance;\nuniform mat4 viewport;\n\nvoid main(){\n	vec3 p0 = (viewport * (gl_in[0].gl_Position / gl_in[0].gl_Position.w)).xyz;\n	vec3 p1 = (viewport * (gl_in[1].gl_Position / gl_in[1].gl_Position.w)).xyz;\n	vec3 p2 = (viewport * (gl_in[2].gl_Position / gl_in[2].gl_Position.w)).xyz;\n\n	// find the altitudes (ha, hb, hc)\n	float a = length(p1 - p2);\n	float b = length(p2 - p0);\n	float c = length(p1 - p0);\n	float alpha = acos((b*b + c*c - a*a) / (2.0*b*c));\n	float beta = acos((a*a + c*c - b*b) / (2.0*a*c));\n	float ha = abs( c * sin(beta));\n	float hb = abs( c * sin(alpha));\n	float hc = abs(b * sin(alpha));\n\n	// send the triangle along with the edge distances\n	edgeDistance = vec3(ha, 0, 0);\n	out_vertex.position = in_vertex[0].position;\n	out_vertex.normal = in_vertex[0].normal;\n	out_vertex.texCoord = in_vertex[0].texCoord;\n	out_vertex.eyes = in_vertex[0].eyes;\n	out_vertex.color = in_vertex[0].color;\n	\n\n	for(int i = 0; i < in_vertex[0].lightDirection.length(); i++){\n		out_vertex.lightDirection[i] = in_vertex[0].lightDirection[i];\n	}\n\n	gl_Position = gl_in[0].gl_Position;\n	EmitVertex();\n\n	edgeDistance = vec3(0, hb, 0);\n	out_vertex.position = in_vertex[1].position;\n	out_vertex.normal = in_vertex[1].normal;\n	out_vertex.texCoord = in_vertex[1].texCoord;\n	out_vertex.color = in_vertex[1].color;\n	out_vertex.eyes = in_vertex[1].eyes;\n	for(int i = 0; i < in_vertex[1].lightDirection.length(); i++){\n		out_vertex.lightDirection[i] = in_vertex[1].lightDirection[i];\n	}\n	gl_Position = gl_in[1].gl_Position;\n	EmitVertex();\n\n	edgeDistance = vec3(0, 0, hc);\n	out_vertex.position = in_vertex[2].position;\n	out_vertex.normal = in_vertex[2].normal;\n	out_vertex.texCoord = in_vertex[2].texCoord;\n	out_vertex.color = in_vertex[2].color;\n	out_vertex.eyes = in_vertex[2].eyes;\n	for(int i = 0; i < in_vertex[2].lightDirection.length(); i++){\n		out_vertex.lightDirection[i] = in_vertex[2].lightDirection[i];\n	}\n	gl_Position = gl_in[2].gl_Position;\n	EmitVertex();\n}\n";
