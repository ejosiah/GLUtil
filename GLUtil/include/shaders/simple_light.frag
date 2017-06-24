uniform struct Light{
    vec4 pos;
    vec4 diff;
    vec4 spec;
} light;

in LightInterface{
    smooth vec3 direction;
    smooth vec3 eyes;
} light_in;


const vec4 globalAmb = vec4(0.2, 0.2, 0.2, 1.0);

vec4 applyLight(vec3 norm, vec4 mColor, float mShine){
    vec4 amb = globalAmb * mColor;
    vec3 N = gl_FrontFacing ? normalize(norm) : normalize(-norm);
    vec3 L = light_in.direction;
    float f = mShine;

    vec4 diff = max(dot(L, N), 0) * light.diff * mColor;

    vec3 E = normalize(light_in.eyes);
    vec3 S = normalize(L + E);
    vec4 spec = pow(max(dot(S, N), 0), f) * light.spec * mColor;

    return amb + diff + spec;
}