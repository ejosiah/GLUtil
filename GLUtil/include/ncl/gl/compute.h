#pragma once

#include <vector>
#include <string>
#include <functional>
#include <glm/glm.hpp>
#include <gl/gl_core_4_5.h>
#include "textures.h"
#include "shader_binding.h"

namespace ncl {
	namespace gl {

		using PreCompute = std::function<void()>;
		using PostCompute = std::function<void()>;
		static std::function<void()> NO_OP = [] {};

		class Compute {
		public:
			Compute() {}

			Compute(glm::ivec3 workers, std::vector<Image2D> images = {}, Shader* shader = nullptr, PreCompute preCompute = NO_OP, PostCompute postCompute = NO_OP)
				:_workers(workers)
				, _shader(shader)
				, _images(images) 
				, _preCompute{ preCompute }
				, _postCompute{ postCompute }
			{

			}

			Compute(glm::ivec3 workers, std::vector<Image2D> images, std::string source, PreCompute preCompute = NO_OP, PostCompute postCompute = NO_OP)
				:Compute(workers, images, from(source), preCompute, postCompute)
			{

			}

			virtual ~Compute() {
				if (_shader) {
					delete _shader;
				}
			}

			static Shader* from(std::string source) {
				Shader* s = new Shader();
				s->loadFromstring(GL_COMPUTE_SHADER, source);
				s->createAndLinkProgram();
				return s;
			}

			virtual void compute() {
				(*_shader)([&] {
					preCompute();
				//	send("dt", dt);
					for (auto& img : _images) {
						img.computeMode();
						img.sendTo(*_shader);
					}
					glDispatchCompute(_workers.x, _workers.y, _workers.z);
				});
				postCompute();
			}

			virtual void preCompute() {
				_preCompute();
			}

			void update(float dt) {
				this->dt = dt;
			}

			virtual void postCompute() {
				_postCompute();
			}

			std::vector<Image2D>& images() {
				return _images;
			}

		protected:
			glm::ivec3 _workers;
			Shader * _shader;
			std::vector<Image2D> _images;
			float dt;
			PreCompute _preCompute;
			PostCompute _postCompute;
		};

		class CheckerBoard_gpu : public Compute {
		public:
			CheckerBoard_gpu(int w = 256, int h = 256, glm::vec4 ca = WHITE, glm::vec4 cb = BLACK, GLuint id = 0, std::string name = "")
				:Compute({ w, h, 1 }, { Image2D(w, h, GL_RGBA32F, name, id) }, sourceTemplate)
				, _w(w), _h(h), _ca(ca), _cb(cb)
			{}

			virtual void preCompute() override {
				send("a", _ca);
				send("b", _cb);
			}

			void updateColorA(const glm::vec4& a) {
				_ca = a;
			}

			void updateColorB(const glm::vec4& b) {
				_cb = b;
			}

		private:
			glm::vec4 _ca;
			glm::vec4 _cb;
			int _w, _h;
			static std::string sourceTemplate;
		};

		std::string CheckerBoard_gpu::sourceTemplate = std::string("#version 430 core\n") +
			"\n" +
			"layout (local_size_x = 1, local_size_y = 1) in;\n" +
			"\n" +
			"layout (rgba32f, binding=0) uniform image2D data;\n" +
			"\n" +
			"uniform vec4 a = vec4(1);\n" +
			"uniform vec4 b = vec4(0);\n" +
			"\n" +
			"void main(void){\n" +
			"\tint i = int(gl_GlobalInvocationID.x);\n" +
			"\tint j = int(gl_GlobalInvocationID.y);\n" +
			"\n" +
			"\tvec4 color = a;\n" +
			"\tif(((i/8)%2 == 0) && ((j/8)%2 == 0) || ((i/8)%2 != 0) && ((j/8)%2 != 0)){\n" +
			"\t\tcolor = b;\n" +
			"\t}\n" +
			"\timageStore(data, ivec2(gl_GlobalInvocationID.xy), color);\n" +
			"}";
	}
}