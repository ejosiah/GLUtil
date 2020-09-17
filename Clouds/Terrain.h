#pragma once

#include "../GLUtil/include/ncl/gl/SceneObject.h"
#include "../GLUtil/include/ncl/geom/Plane.h"
//#include "../GLUtil/include/ncl/units/units.h"
#include <glm/glm.hpp>
#include <optional>
#include <vector>

using namespace ncl;
using namespace gl;
using namespace glm;

class Terrain : public SceneObject {
public:
	Terrain(Scene& scene, float _length, float numGrids, std::optional<std::string> heightMapPath = {}, std::optional<std::string> normalMapPath = {})
		:SceneObject(&scene)
		,_length(_length)
		, _numGrids(numGrids)
		, generated(!heightMapPath.has_value())

	{
		if (!generated) {
			initHeightMap(*heightMapPath, *normalMapPath);
		}
		init();
	}

	void init() override {
		vec4 outer{ _numGrids };
		vec2 inner{ _numGrids };
		glPatchParameteri(GL_PATCH_VERTICES, 4);

		glPatchParameterfv(GL_PATCH_DEFAULT_OUTER_LEVEL, &outer[0]);
		glPatchParameterfv(GL_PATCH_DEFAULT_INNER_LEVEL, &inner[0]);

		Mesh mesh;
		mesh.positions.emplace_back(-_length, 0, _length);
		mesh.positions.emplace_back(_length, 0, _length);
		mesh.positions.emplace_back(_length, 0, -_length);
		mesh.positions.emplace_back(-_length, 0, -_length);
		mesh.primitiveType = GL_PATCHES;

		patch = ProvidedMesh{ mesh };
		std::vector<vec4> v;
		v.resize(100);
		
		//patchBuffer = new TextureBuffer{ "patches", &v[0], sizeof(vec4) * 100 };
		//patch.enableTransformFeedBack(patchBuffer->buffer());

	}

	void initHeightMap(std::string path, std::string normalMapPath) {
		if (!generated) {
			heightMap = Texture2D{ path , 0, "heightMap"};
			normalMap = Texture2D{ normalMapPath , 1, "normalMap"};
		}
	}

	void render(bool shadowMode = false) {
		scene().shader("terrain")([&] {
		//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glBindTextureUnit(0, heightMap.buffer());
			glBindTextureUnit(1, normalMap.buffer());

			send("generated", generated);
			send(scene().activeCamera());
			send("camPos", scene().activeCamera().getPosition());
			send("sunPos", vec3(50000));
			shade(patch);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		});
		//scene().shader("face_normal")([&] {
		//	send(scene().activeCamera());
		//	send("normal_length", 10.0f);
		//	shade(patch);
		//});
	}

private:
	float _length;
	float _numGrids;
	bool generated;
	ProvidedMesh patch;
	Texture2D heightMap;
	Texture2D normalMap;
	TextureBuffer* patchBuffer;

};