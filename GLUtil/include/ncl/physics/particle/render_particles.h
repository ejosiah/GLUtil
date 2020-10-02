#pragma once

#include <memory>
#include <vector>
#include <utility>
#include <gl/gl_core_4_5.h>
#include <glm/glm.hpp>
#include "../../gl/Drawable.h"
#include "../../gl/Shader.h"
#include "../../util/numeric_iterator.h"

namespace ncl {
	namespace physics {
		namespace pm {
			class Particles : public gl::Drawable, gl::CopyBuffer {
			public:
				Particles() = default;

				Particles(int count, gl::Shape& shape)
					:_count{ count }
					, _primitiveType{shape.getPrimitiveType()}
				{
					init(nullptr, &shape);
				}

				Particles(std::vector<glm::vec4> positions, gl::Shape& shape)
					:_count{ int(positions.size()) }
					, _primitiveType{ shape.getPrimitiveType() }
				{
					init(&positions[0], &shape);
				}

				Particles(int count, glm::vec3 initialPos = glm::vec3(0))
					:_count{ count }
					, _primitiveType{GL_POINTS}
				{
					init(nullptr, nullptr, glm::vec4(initialPos, 1));
				}

				Particles(std::vector<glm::vec4> positions, glm::vec3 initialPos = glm::vec3(0))
					:_count{ int(positions.size()) } 
				{
					init(&positions[0], nullptr, glm::vec4(initialPos, 1));
				}

				void init(glm::vec4* data, gl::Shape* shape, glm::vec4 initialPosition = glm::vec4(0, 0, 0, 1)) {
					glGenBuffers(3, _buffers);

					glBindBuffer(GL_ARRAY_BUFFER, _buffers[0]);
					glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * _count, data, GL_STATIC_DRAW);

					glBindBuffer(GL_ARRAY_BUFFER, _buffers[1]);
					glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * _count, data, GL_STATIC_DRAW);


					std::vector<glm::vec4> positions{ initialPosition };

					if (shape) {
						positions.clear();
						auto mesh = shape->getMeshes()[0];
						auto meshPos = mesh.positions;
						for (int i = 0; i < meshPos.size(); i++) {
							positions.push_back(glm::vec4(meshPos[i], 1));
						}

					}

					_num_elements = positions.size();
					glBindBuffer(GL_ARRAY_BUFFER, _buffers[2]);
					glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * positions.size(), &positions[0], GL_STATIC_DRAW);

					glGenVertexArrays(2, _vaos);

					for (int i = 0; i < 2; i++) {
						uint_iterator attribIndex;
						glBindVertexArray(_vaos[i]);
						glBindBuffer(GL_ARRAY_BUFFER, _buffers[2]);
						glEnableVertexAttribArray(*attribIndex);
						glVertexAttribPointer(*attribIndex, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

						++attribIndex;
						glBindBuffer(GL_ARRAY_BUFFER, _buffers[i]);
						glEnableVertexAttribArray(*attribIndex);
						glVertexAttribPointer(*attribIndex, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
						glVertexAttribDivisor(*attribIndex, 1);

						if (shape) {

							if (shape->hasIndices()) {
								_num_elements = shape->numIndices();
								_has_indices = true;
								auto buffer = shape->copyIndices();
								glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
								_attribs.push_back(buffer);
							}
						}
					}
					glBindVertexArray(0);
				}

				Particles(const Particles&) = delete;

				Particles(Particles&& source)  noexcept {
					transfer(source, *this);
				}

				Particles& operator=(const Particles&) = delete;

				Particles& operator=(Particles&& source) noexcept {
					transfer(source, *this);
					return *this;
				}

				~Particles() {
					//glDeleteBuffers(3, _buffers);
					//glDeleteBuffers(_attribs.size(), &_buffers[0]);
					//glDeleteVertexArrays(2, _vaos);
				}

				void swapBuffers() {
					std::swap(_front, _back);
				}

				constexpr GLuint front() const {
					return _buffers[_front];
				}

				constexpr GLuint back() const {
					return _buffers[_back];
				}

				void draw(gl::Shader& shader) override {
					glBindVertexArray(_vaos[_front]);
					if (_has_indices) {
						
						glDrawElementsInstanced(_primitiveType, _num_elements, GL_UNSIGNED_INT, 0, _count);
					}
					else {
						glDrawArraysInstanced(_primitiveType, 0, _num_elements, _count);
					}
					glBindVertexArray(0);
				}

				static inline void transfer(Particles& source, Particles& dest) {
					dest._count = source._count;
					dest._vaos[0] = source._vaos[0];
					dest._vaos[1] = source._vaos[1];
					dest._buffers[0] = source._buffers[0];
					dest._buffers[1] = source._buffers[1];
					dest._buffers[2] = source._buffers[2];
					dest._front = source._front;
					dest._back = source._back;
					dest._primitiveType = source._primitiveType;
					dest._num_elements = source._num_elements;
					dest._has_indices = source._has_indices;
					dest._attribs = std::move(source._attribs);

					source._vaos[0] = 0;
					source._vaos[1] = 0;

					source._buffers[0] = 0;
					source._buffers[1] = 0;
					source._buffers[2] = 0;
				}

			private:
				int _count = 0;
				GLuint _vaos[2] = { 0, 0 };
				GLuint _buffers[3] = { 0, 0, 0 };
				std::vector<GLuint> _attribs = {};
				int _front = 0;
				int _back = 1;
				GLenum _primitiveType = GL_POINTS;
				GLsizei _num_elements = 0;
				bool _has_indices = false;

			};
		}
	}
}