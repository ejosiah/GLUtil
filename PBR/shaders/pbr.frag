#version 450 core
#pragma debug(on)
#pragma optimize(off)

layout(binding = 0) uniform sampler2D albedo;
layout(binding = 1) uniform sampler2D normal;
layout(binding = 2) uniform sampler2D metallic;
layout(binding = 3) uniform sampler2D roughness;
layout(binding = 4) uniform sampler2D ao;