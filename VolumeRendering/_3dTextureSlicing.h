#pragma once

#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/gl/FrameBuffer.h"
#include "VolumeRenderingObject.h"
#include <glm/glm.hpp>

using namespace std;
using namespace glm;
using namespace ncl;
using namespace gl;

class _3dTextureSlicing : public VolumeRenderingObject {
public:
	_3dTextureSlicing(Scene* scene) 
		: VolumeRenderingObject{ scene, "3D slicing Volume Rendering" }
	{
		init();
	}

	void init() override {
		Mesh mesh;
		mesh.positions.resize(12);
		textureSlices = ProvidedMesh{ mesh,  false, (unsigned)MAX_SLICES };
		textureSlices.defautMaterial(false);
//		slices = StorageBuffer<vec3>{ textureSlices, 0 };

		
		slices = StorageBuffer<int>{ size_t{4096}, 0 };

		int workers = (int)(std::ceil(MAX_SLICES / 512.0));

		sliceVolumeOp = new Compute{ {workers, 1, 1}, {}, &scene().shader("slice_volume"),
		[&] {
				send("viewDir", viewDir);
				send("num_slices", num_slices);
			}
		};
	//	scene().addCompute(sliceVolumeOp);
		cube = Cube{ 1 };
		cube.defautMaterial(false);
		initFB();
	}



	void initFB() {
		auto attachment = gl::FrameBuffer::Attachment{};
		attachment.texTarget = GL_TEXTURE_3D;
		attachment.internalFmt = GL_RED;
		attachment.fmt = GL_RED;
		attachment.type = GL_UNSIGNED_BYTE;
		attachment.wrap_s = attachment.wrap_t = attachment.wrap_r = GL_CLAMP_TO_EDGE;
		attachment.numLayers = ZDIM;

		auto config = gl::FrameBuffer::defaultConfig(XDIM, YDIM);
		config.attachments[0] = attachment;
		//config.depthAndStencil = false;
		//config.depthTest = false;

		fb = gl::FrameBuffer{ config };
	}

	bool once = true;
	void render(bool shadowMode) override {

		//fb.use([&] {
			scene().shader("slicer")([&] {
				send(slices);
				send(scene().activeCamera(), scale(mat4(1), vec3(6)));
				send("viewDir", viewDir);
				send("num_slices", num_slices);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				//glBindTextureUnit(0, texture->buffer());
				glBindTextureUnit(0, noiseTexture->buffer());
				shade(textureSlices);
				glDisable(GL_BLEND);
				});
	//		});

		//if (once) {
		//	once = false;
		//	slices.read([&](int* ptr) {
		//		for (int i = 0; i < 128; i++) {
		//		//	logger.info("slice[" + to_string(i) + "]: " + to_string(*(ptr + i)));
		//		}
		//		});
		//}
		//scene().renderText(10, 20, to_string(num_slices) + " slices");
		//scene().renderText(scene().width() - 200, 10, "+ for more slices, - for less slices");

		scene().shader("flat")([&] {
			glBindTextureUnit(0, fb.texture());
			glBindTextureUnit(1, texture->buffer());
			shade(cube);
		});
	}

	void update(float dt) override {;
		viewDir = scene().activeCamera().getViewDirection();
	}

	void processInput(const Key& key) override {
		if (key.pressed()) {
			if (key.value() == '=') {
				num_slices *= 2;
			}
			else if(key.value() == '-') {
				num_slices /= 2;
			}
			num_slices = glm::clamp(num_slices, 8, MAX_SLICES);
		}
	}


private:
	const int MAX_SLICES = 4096;
	ProvidedMesh textureSlices;
//	StorageBuffer<vec3> slices;
	StorageBuffer<int> slices;
	Cube cube;
	int num_slices = 256;
	Compute* sliceVolumeOp;
	vec3 viewDir;
	FrameBuffer fb;
	Logger logger = Logger::get("3d Slicing");
};
