#pragma once

#include <gl/gl_core_4_5.h>
#include <string>
#include <stdexcept>
#include <fstream>
#include <IL/devil_cpp_wrapper.hpp>
#pragma comment(lib, "ILUT.lib")
#pragma comment(lib, "ILU.lib")
#pragma comment(lib, "DevIL.lib")

namespace ncl {
	namespace gl {
		
		static bool devilInit = false;

		class Image {
			enum Status { Success = 0, Failure };
		private:
			unsigned int id;
		public:
			Status status;

		public:
			Image() = default;

			Image(std::string filename, ILenum dataType = IL_UNSIGNED_BYTE,  ILenum origin = IL_ORIGIN_LOWER_LEFT) {
				if (!devilInit) {
					ilInit();
				}
				ilGenImages(1, &id);
				ilBindImage(id);
				ilEnable(IL_ORIGIN_SET);
				ilOriginFunc(origin);
				ILboolean success = ilLoadImage(filename.c_str());
				if (success) {
					success = ilConvertImage(IL_RGBA, dataType);
					status = success ? Success : Failure;
				}
				else {
					ilDeleteImages(1, &id);
					ILenum error = ilGetError();
					intepretError(error);
					status = Failure;
				}
			}

			~Image() {
				ilDeleteImages(1, &id);
			}

			int width() const {
				return ilGetInteger(IL_IMAGE_WIDTH);
			}

			int height() const {
				return ilGetInteger(IL_IMAGE_HEIGHT);
			}

			unsigned char* data() {
				return ilGetData();
			}

			void intepretError(ILenum error) {
				switch (error) {
				default:
					break;
				}
			}
		};

		inline ILenum glTypeToILType(GLenum type) {
			switch (type) {
			case GL_UNSIGNED_INT:
				return IL_UNSIGNED_INT;
			case GL_BYTE:
				return IL_BYTE;
			case GL_SHORT:
				return IL_SHORT;
			case GL_INT:
				return IL_INT;
			case GL_FLOAT:
				return IL_FLOAT;
			case GL_DOUBLE:
				return IL_DOUBLE;
			default:
				throw "invalid GL data type: " + std::to_string(type);
			}
		}
	}
}