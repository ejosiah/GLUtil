vec4 diffuseContrib(vec3 L, vec3 N, LightSource light, Material m){
	return  max(dot(L, N), 0)  * light.diffuse * getDiffuse(m);
}