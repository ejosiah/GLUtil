#pragma once


vec2 hammersley2d(uint i, const uint N) {
    vec2 P;
    P.x = float(i) * (1.0 / float(N));

    i = (i << 16u) | (i >> 16u);
    i = ((i & 0x55555555u) << 1u) | ((i & 0xAAAAAAAAu) >> 1u);
    i = ((i & 0x33333333u) << 2u) | ((i & 0xCCCCCCCCu) >> 2u);
    i = ((i & 0x0F0F0F0Fu) << 4u) | ((i & 0xF0F0F0F0u) >> 4u);
    i = ((i & 0x00FF00FFu) << 8u) | ((i & 0xFF00FF00u) >> 8u);
    P.y = float(i) * 2.3283064365386963e-10; // / 0x100000000

    return P;
}


vec3 pointInSphere(vec2 uv) {
    const float PI = 3.1415926535897932384626422832795028841971;

    float sin0 = sqrt(1 - uv.x * uv.x);
    float phi = 2 * PI * uv.y;
    return vec3(
        cos(phi) * sin0,
        sin(phi) * sin0,
        cos(uv.x)
    );
}

vec3 hammersleySphere(int i, uint N) {
    vec2 uv = hammersley2d(i, N);
    return pointInSphere(uv);
}