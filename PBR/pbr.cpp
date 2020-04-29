#include "../GLUtil/include/ncl/configured_logger.h"
#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "../GLUtil/include/ncl/gl/Resolution.h"
#include "../GLUtil/include/ncl/gl/pbr.h"
#include "pbrScene.h"
#include <iostream>

int main() {
	auto scene = new PbrScene;
	start(scene);

	//cout << "sizeof int: " << sizeof(int) << endl;
	//cout << "alignment of int: " << alignof(int) << endl;
	//cout << "sizeof Unit: " << sizeof(Unit) << endl;
	//cout << "alignment of Unit: " << alignof(Unit) << endl;
	//cout << "sizeof LightType: " << sizeof(pbr::LightType) << endl;
	//cout << "alignment of LightType: " << alignof(pbr::LightType) << endl;

	//Unit unit = Unit::Power;
	//int* i =   (int*)unit;
	//
	//cout << *i << endl;

	//cin.get();

	return 0;
}