// MacbethChart.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "../GLUtil/include/ncl/configured_logger.h"
#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "../GLUtil/include/ncl/gl/Resolution.h"
#include "MacBethChartScene.h"

int main() {
	auto scene = new MacBethChartScene;
	start(scene);
	//auto m =  translate(mat4(1), { 3, 2, 4 });
	//auto id = mat4(
	//	1, 0, 0, 0,
	//	0, 1, 0, 0,
	//	0, 0, 1, 0,
	//	3, 2, 4, 1
	//);

	//auto x = row(id, 0);
	//auto y = row(id, 1);
	//auto z = row(m, 2);

	//auto v = vec4(0, 0, 0, 1);

	//auto dx = m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2] + m[0][3] * v[3];
	//auto dy = m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2] + m[1][3] * v[3];
	//auto dz = m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2] + m[2][3] * v[3];
	//auto dw = m[3][0] * v[0] + m[3][1] * v[1] + m[3][2] * v[2] + m[3][3] * v[3];

	//vec4 t = m * v;
	//vec3 t0 = mat3(1) * v.xyz;
	return 0;
}

