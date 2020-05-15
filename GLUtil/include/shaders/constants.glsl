const int MAX_LIGHT_SOURCES = 10;
const float PI = 3.1415926535897932384626422832795028841971;
const float INV_PI = 0.31830988618379067153776752674503;
const float TWO_PI = 6.2831853071795864769252867665590057683943;
const float INV_TWO_PI = 0.15915494309189533576888376337251;
const float FOUR_PI = 12.566370614359172953850573533118;
const float INV_FOUR_PI = 0.07957747154594766788444188168626;
const float SQRT_OF_ONE_THIRD = 0.5773502691896257645091487805019574556476;
const float SQRT_OF_TWO = 1.4142135623730950488016887242097;
const float EPSILON = 0.000001;
const vec3 dielectric = vec3(0.04);
const float MAX_REFLECTION_LOD = 4.0;


// Spherical Harmonics constants
const int Y00 = 0; // Y00
const int Y1_1 = 1;
const int Y10 = 2;
const int Y11 = 3;
const int Y2_2 = 4;
const int Y2_1 = 5;
const int Y20 = 6;
const int Y21 = 7;
const int Y22 = 8;