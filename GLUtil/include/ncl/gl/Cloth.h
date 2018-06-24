#pragma once

#include "Plane.h"
#include "TransformFeedBack.h"
#include "textures.h"

namespace ncl {
	namespace gl {
		class Cloth : public Drawable {
		public:
			Cloth() {
				glGenVertexArrays(2, vaoUpdateID);
				glGenBuffers(2, vboID_PrePos);
				for (int i = 0; i < 2; i++) {
					plane[i] = new Plane(numY, numX, sizeX, sizeY);

					vboID_Pos[i] = plane[i]->getBuffers()[0][0];

					GLint size;
					glBindBuffer(GL_COPY_READ_BUFFER, vboID_Pos[i]);
					glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_SIZE, &size);

					glBindBuffer(GL_COPY_WRITE_BUFFER, vboID_PrePos[i]);
					glBufferData(GL_COPY_WRITE_BUFFER, size, nullptr, GL_DYNAMIC_COPY);
					glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, size);

					GLint size;
					glBindVertexArray(vaoUpdateID[i]);
					glBindBuffer(GL_ARRAY_BUFFER, vboID_Pos[i]);
					glEnableVertexAttribArray(0);
					glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

					glBindBuffer(GL_ARRAY_BUFFER, vboID_PrePos[i]);
					glEnableVertexAttribArray(1);
					glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
									
				}

				glBindVertexArray(0);

				posBuffer = new DoubleBuffer(0, GL_RGBA32F, vboID_Pos);
				prevPosBuffer = new DoubleBuffer(1, GL_RGBA32F, vboID_PrePos);
			}

			void initClothShader() {
				(*clothShader)([&](Shader& s) {
					s.addAttribute("position_mass", 0);
					s.addAttribute("prev_position", 1);
					s.sendUniform3fv("gravity", 1, &gravity.x);
					s.sendUniform1i("tex_position_mass", 0);
					s.sendUniform1i("tex_pre_position_mass", 1);
					s.sendUniform1i("texsize_x", texture_size_x);
					s.sendUniform1i("texsize_y", texture_size_y);
					s.sendUniform2f("inv_cloth_size", float(sizeX) / numX, float(sizeY) / numY);
					s.sendUniform2f("step", 1.0f / (texture_size_x - 1.0f), 1.0f / (texture_size_y - 1.0f));
					s.sendUniform1f("ksStr", KsStruct);
					s.sendUniform1f("ksShr", KsShear);
					s.sendUniform1f("ksBnd", KsBend);
					s.sendUniform1f("kdStr", KdStruct / 1000.0f);
					s.sendUniform1f("kdShr", KdShear / 1000.0f);
					s.sendUniform1f("kdBnd", KdBend / 1000.0f);
					s.sendUniform1f("DEFAULT_DAMPING", DEFAULT_DAMPING);
				});

			}


		virtual void draw(Shader& shader) override {
			plane[posBuffer->front()]->draw(shader);
		}

		void update(float dt) {
			for (int i = 0; i < NUM_ITER; i++) {
				posBuffer->activate();
				prevPosBuffer->activate();

				glBindVertexArray(vaoUpdateID[posBuffer->front()]);
				GLuint bufrers[] = { vboID_Pos[posBuffer->back()] , vboID_PrePos[prevPosBuffer->back()] };
				(*tfb)(bufrers, 2, GL_POINTS, [&]() {
					glDrawArrays(GL_POINTS, 0, total_points); 
				});
				posBuffer->swapBuffers();
				prevPosBuffer->swapBuffers();
			}
			glBindVertexArray(0);
		}
			
		private:
			Plane* plane[2];
			Shader* clothShader;
			TransformFeebBack* tfb;
			DoubleBuffer* posBuffer;
			DoubleBuffer* prevPosBuffer;
			GLuint	vboID_Pos[2], vboID_PrePos[2];
			GLuint vaoUpdateID[2], vaoRenderID[2];
			int texture_size_x = 0;
			int texture_size_y = 0;
			const float DEFAULT_DAMPING = -0.05f;
			float	KsStruct = 10.5f, KdStruct = -5.5f;
			float	KsShear = 0.25f, KdShear = -0.25f;
			float	KsBend = 0.25f, KdBend = -0.25f;

			//total number of particles on X and Z axis
			int numX = 21, numY = 21;
			const int total_points = (numX + 1)*(numY + 1);

			//world space cloth size
			int sizeX = 4,
				sizeY = 4;
			float hsize = sizeX / 2.0f;

			//number of transform feedback iterations
			const int NUM_ITER = 10;

			//default gravity and mass 
			glm::vec3 gravity = glm::vec3(0.0f, -0.00981f, 0.0f);
			float mass = 1.0f;
		};
	}
}