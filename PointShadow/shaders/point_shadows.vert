//#version 450 core
//layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aNormal;
//layout (location = 2) in vec2 aTexCoords;
//
//out vec2 TexCoords;
//
//out VS_OUT {
//    vec3 FragPos;
//    vec3 Normal;
//    vec2 TexCoords;
//} vs_out;
//
//uniform mat4 projection;
//uniform mat4 view;
//uniform mat4 model;
//
//uniform bool reverse_normals;
//
//void main()
//{
//    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
//    if(reverse_normals) // a slight hack to make sure the outer large cube displays lighting from the 'inside' instead of the default 'outside'.
//        vs_out.Normal = transpose(inverse(mat3(model))) * (-1.0 * aNormal);
//    else
//        vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
//    vs_out.TexCoords = aTexCoords;
//    gl_Position = projection * view * model * vec4(aPos, 1.0);
//}

#version 450 core
layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec3 tangent;
layout(location=3) in vec3 bitangent;
layout(location=4) in vec4 color;
layout(location=5) in vec2 uv;
layout(location=8) in mat4 xform;

out vec2 TexCoords;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat4 MVP;

uniform bool reverse_normals;

void main()
{
    vs_out.FragPos = vec3(M * xform * vec4(position, 1.0));
    if(reverse_normals) // a slight hack to make sure the outer large cube displays lighting from the 'inside' instead of the default 'outside'.
        vs_out.Normal = transpose(inverse(mat3(M * xform))) * (-1.0 * normal);
    else
        vs_out.Normal = transpose(inverse(mat3(M * xform))) * normal;
    vs_out.TexCoords = uv;
    gl_Position = P * V * M * xform * vec4(position, 1.0);
}