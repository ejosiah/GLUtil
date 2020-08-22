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
	}

	inline string name() {
		return _name;
	}

protected:
	const std::string volume_file = "media/Engine256.raw";
	GLuint XDIM = 256;
	GLuint YDIM = 256;
	GLuint ZDIM = 256;
	Texture3D* texture;
	string _name;
};