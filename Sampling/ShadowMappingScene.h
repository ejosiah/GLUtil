#pragma once
#include "../GLUtil/include/ncl/gl/Scene.h"

using namespace std;
using namespace glm;
using namespace ncl;
using namespace gl;

class ShadowMappingScene : public Scene {
public:
	ShadowMappingScene():Scene("ShadowMapping"){}
};