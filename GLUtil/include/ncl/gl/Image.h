#pragma once
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
			Image(std::string filename, ILenum origin = IL_ORIGIN_LOWER_LEFT) {
				if (!devilInit) {
					ilInit();
				}
				ilGenImages(1, &id);
				ilBindImage(id);
				ilEnable(IL_ORIGIN_SET);
				ilOriginFunc(origin);
				ILboolean success = ilLoadImage(filename.c_str());
				if (success) {
					success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
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
	}
}