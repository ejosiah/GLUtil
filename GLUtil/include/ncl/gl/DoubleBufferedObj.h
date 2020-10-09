#pragma once

#include <initializer_list>
#include <vector>
#include <functional>
#include "Shape.h"
#include "textures.h"
#include "CopyBuffer.h"

namespace ncl {
	namespace gl {
		class DoubleBuffered : public CopyBuffer {
		public:
			using Proc = std::function<void(GLuint*, GLuint*, int)>;

			/*
			virtual ~DoubleBuffered() {
				auto no_of_meshes = self()->numMeshes();
				for (int i = 0; i < no_of_meshes; i++) {
					for (int j = 0; i < attributes().size(); j++) {
						glDeleteBuffers(1, &vboIds[i][j][1]);  // delete only the our copy
						delete[] vboIds[i][j];
					}
					glDeleteVertexArrays(1, &vaoIds[i][1]); // delete only the our copy
					delete[] vaoIds[i];
				}
			}*/

			virtual Shape* self() = 0;

			virtual std::vector<int> attributes() const = 0;

			virtual std::vector<int> copyFrom() const {
				return {};
			}

			virtual void enableDoubleBuffering() {
				auto shape = self();
				auto no_of_meshes = shape->numMeshes();
				_fronts = std::vector<int>( no_of_meshes, 0 );
				_backs = std::vector<int>( no_of_meshes, 1 );

				for (int i = 0; i < no_of_meshes; i++) {

					GLuint* vaos = new GLuint[2];

					vaos[0] = shape->getVaoId(i);
					glGenVertexArrays(1, &vaos[1]);

					glBindVertexArray(vaos[1]);

					std::vector<GLuint*> meshBuffers;
					for (int j = 0; j < attributes().size(); j++) {
						GLuint* vbos = new GLuint[2];
						auto attribute = attributes()[j];

						auto source = copyFrom().empty() ? attribute : copyFrom()[j];
						
						vbos[0] = shape->getBuffers()[i][attribute];
						vbos[1] = std::get<Buffer>(copy(shape->getBuffers()[i][source]));

						glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
						glEnableVertexAttribArray(attribute);
						glVertexAttribPointer(j, 3, GL_FLOAT, GL_FALSE, 0, 0);	// based on attribute, 
						meshBuffers.push_back(vbos);
					}
					vaoIds.push_back(vaos);
					vboIds.push_back(meshBuffers);
					
					glBindVertexArray(0);
					
				}
			}

			// CHANGE this to only create double buffers
			// other logic to xforms f
			virtual void useDoubleBuffer(Proc proc) {
				auto no_of_buffers = attributes().size();
				auto no_of_meshes = vaoIds.size();

				for (int i = 0; i < no_of_meshes; i++) {
					auto vaos = vaoIds[i];
					auto mBuffers = vboIds[i];
					glBindVertexArray(vaos[_fronts[i]]);
					GLuint* front_bufs = new GLuint[no_of_buffers];
					GLuint* back_bufs = new GLuint[no_of_buffers];
					for (int j = 0; j < no_of_buffers; j++) {
						front_bufs[j] = mBuffers[j][_fronts[i]];
						back_bufs[j] = mBuffers[j][_backs[i]];
					}
					proc(front_bufs, back_bufs, no_of_buffers);
					glBindVertexArray(0);
					delete[] front_bufs;
					delete[] back_bufs;
					swapBuffers(i);
				}
			}

			void swapBuffers(int index = 0) {
				std::swap(_fronts[index], _backs[index]);
			}

			GLuint front(int index = 0) const {
				return _fronts[index];
			}

			GLuint back(int index = 0) const {
				return _backs[index];
			}

			GLuint* frontBufs(int index = 0) {

			}

			GLuint* backBufs(int index = 0) {

			}

		protected:
			std::vector<GLuint*> vaoIds;
			std::vector<int> _fronts;
			std::vector<int> _backs;
			std::vector<std::vector<GLuint*>> vboIds;
		};
	}
}