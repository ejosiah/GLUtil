#pragma once
#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/gl/SceneObject.h"
#include "../GLUtil/include/ncl/gl/FrameBuffer.h"
#include <glm/glm.hpp>

using namespace std;
using namespace glm;
using namespace ncl;
using namespace gl;

class VolumeRenderingObject : public SceneObject {
public:
	VolumeRenderingObject(Scene* scene, string name) 
		:SceneObject{ scene } 
		, _name{ name }
	{
		init();
	}

	virtual void init() override {
		std::ifstream infile(volume_file.c_str(), std::ios_base::binary);
		if (infile.good()) {
			GLubyte* pData = new GLubyte[XDIM * YDIM * ZDIM];
			infile.read(reinterpret_cast<char*>(pData), XDIM * YDIM * ZDIM * sizeof(GLubyte));
			infile.close();

			// 			Texture3D(void* data, GLuint width, GLuint height, GLuint depth, GLuint id = nextId++, GLuint iFormat = GL_RGBA8, GLuint format = GL_RGBA, glm::vec2 wrap = glm::vec2{ GL_REPEAT }, glm::vec2 minMagfilter = glm::vec2{ GL_LINEAR }, GLenum type = GL_UNSIGNED_BYTE, std::function<void()> extraOptions = [] {}) : _id(id) {

			texture = new Texture3D{
				(void*)pData,
				XDIM,
				YDIM,
				ZDIM,
				0,
				GL_RED,
				GL_RED,
				vec3{GL_CLAMP_TO_EDGE},
				{GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR},
				GL_UNSIGNED_BYTE,
				[] {
						glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0);
						glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, 4);
						glGenerateMipmap(GL_TEXTURE_3D);
					}
			};
		//	CHECK_GL_ERRORS

		}
		else {
			throw std::runtime_error{ "unable to load volume file: " + volume_file };
		}

		noiseTexture = new Texture3D{
			nullptr,
			XDIM,
			YDIM,
			ZDIM,
			0,
			GL_RGBA32F,
			GL_RGBA,
			glm::vec3{ GL_REPEAT },
			glm::vec2{ GL_LINEAR },
			GL_FLOAT
		};

		scene().shader("noise")([&] {
			glBindImageTexture(image, noiseTexture->buffer(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
			glDispatchCompute(workers.x, workers.y, workers.z);
			});
	}

	inline string name() {
		return _name;
	}

protected:
	const std::string volume_file = "media/Engine256.raw";
	const GLuint XDIM = 256;
	const GLuint YDIM = 256;
	const GLuint ZDIM = 256;
	Texture3D* texture;
	string _name;
	Texture3D* noiseTexture;
	GLuint image = 0;
	uvec3 workers = uvec3(XDIM, YDIM, ZDIM) / uvec3(8, 8, 8);
};