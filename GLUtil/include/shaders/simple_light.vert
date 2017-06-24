uniform struct Light{
    vec4 pos;
    vec4 diff;
    vec4 spec;
} light;

out LightInterface{
    smooth vec3 direction;
    smooth vec3 eyes;
} light_out;


void doLight(vec4 pos, mat4 M){
    light_out.direction = ( light.pos).xyz;
    light_out.eyes = vec3(0, 0, 1);
}