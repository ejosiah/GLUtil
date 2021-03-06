#pragma once


#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <thread>
#include <future>
#include <gl/gl_core_4_5.h>
#include <glm/gtc/type_ptr.hpp>
#include "models.h"
#include "Scene.h"
#include "common.h"
#include "Shader.h"
#include "shaders.h"
#include "logger.h"
#include <ft2build.h>
#include "Image.h"
#include <sstream>
#include <cstdint>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include FT_FREETYPE_H

#pragma comment(lib, "freetype.lib")

namespace ncl {
	namespace gl {

		struct Character {
			unsigned int id;
			glm::ivec2 size;
			glm::ivec2 bearing;
			unsigned int advance;
		};

		void cleanup(GLuint* id, glm::vec4** boxes, unsigned size) {
			delete[] id;
			for (int i = 0; i < size; i++) {
				delete[] boxes[i];
			}
			delete[] boxes;
		}

		class Font {
		private:
			Font(std::string name, int size = 10, int style = 0, const color& color = WHITE) {
				this->size = size;
				this->style = style;
				this->color = color;
				auto font = getFontName(name, style);
				const char* path = location[font].c_str();

				int error = FT_New_Face(library, path, 0, &face);

				if (error) {
					intepretError(error);
				}
				FT_Set_Pixel_Sizes(face, 0, size);
				loadFont();
			}

			void loadFont() {
				for (unsigned char c = 0; c < 128; c++) {
					if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
						logger.warn("unable to load char"); // TODO add char to msg
						continue;
					}

					unsigned int texture;
					glGenTextures(1, &texture);
					glBindTexture(GL_TEXTURE_2D, texture);
					auto g = face->glyph;
					maxHeight = std::max(maxHeight, g->bitmap.rows);
					maxWidth = std::max(maxWidth, g->bitmap.width);
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, g->bitmap.width, g->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					Character ch{
						texture,
						glm::ivec2(g->bitmap.width, g->bitmap.rows),
						glm::ivec2(g->bitmap_left, g->bitmap_top),
						g->advance.x
					};
					character[c] = ch;
					glBindTexture(GL_TEXTURE_2D, 0);
				}
			}
		public:
			static const int BOLD;
			static const int ITALIC;

			static Font* Arial(int size = 10, int style = 0, const color& color = BLACK) {
				return getFont("Arial", size, style, color);
			}

			static Font* Courier(int size = 10, int style = 0, const color& color = BLACK) {
				return getFont("Courier", size, style, color);
			}

			static Font* getFont(std::string name, int size, int style, const color& color) {
				auto colorStr = std::to_string(color.r) + std::to_string(color.g) + std::to_string(color.b);
				auto key = getFontName(name, style) + std::to_string(size) + colorStr;
				auto itr = fonts.find(key);
				if (itr == fonts.end()) {
					fonts[key] = new Font(name, size, style, color);
				}
				return fonts[key];
			}

			static void onResize(int w, int h) {
				using namespace std;
				for_each(fonts.begin(), fonts.end(), [&](std::pair<string, Font*> pair) { pair.second->resize(w, h); });
			}

			void intepretError(int code) {
				std::string msg = "unable to load font, reason: ";
				switch (code) {
				case FT_Err_Unknown_File_Format:
					throw std::runtime_error(msg + "unknown file format");
				case FT_Err_Cannot_Open_Resource:
					throw std::runtime_error(msg + "unable to open resource");
				}
			}

			void useBackground(glm::vec4& color) {
				background = new glm::vec4(color);
			}

			void render(const std::string& text, float x, float y, float d = -1) {				
				y = _height - y;

				shader.use();
				glActiveTexture(GL_TEXTURE10);
				shader.sendUniform1i("glyph", 10);
				shader.sendUniform4fv("color", 1, glm::value_ptr(color));
				bool blendingOff = !glIsEnabled(GL_BLEND);
				bool depthTestOn = glIsEnabled(GL_DEPTH_TEST);
				if (blendingOff) glEnable(GL_BLEND);
				if (depthTestOn) glDisable(GL_DEPTH_TEST);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				GLint polygonMode[2];
				glGetIntegerv(GL_POLYGON_MODE, polygonMode);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

				withVertexArray(vaoId, [&]() {
					float startX = x;
					for (char c : text) {

						if (c == '\n') {
							y += (maxHeight + 2) * d;
							x = startX;
							continue;
						}
						if (c == '\t') {
							x += maxWidth * 2;
							continue;
						}

						auto ch = character[c];
						glBindTexture(GL_TEXTURE_2D, ch.id);

						float x2 = x + ch.bearing.x;
						float y2 = y - (ch.size.y - ch.bearing.y);
						float w = ch.size.x;
						float h = ch.size.y;

						box[0] = { x2, y2 + h    , 0, 0 };;
						box[1] = { x2, y2, 0, 1 };
						box[2] = { x2 + w, y2 + h, 1, 0 };
						box[3] = { x2 + w, y2    , 1, 1 };

						glBindBuffer(GL_ARRAY_BUFFER, bufferId);
						glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(box), box);
						glBindBuffer(GL_ARRAY_BUFFER, 0);

						glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

						x += (ch.advance >> 6);
					}
					
				});
				if (blendingOff) glDisable(GL_BLEND);
				if (depthTestOn) glEnable(GL_DEPTH_TEST);
				glPolygonMode(GL_FRONT_AND_BACK, polygonMode[0]);
				glActiveTexture(GL_TEXTURE0);

				shader.unUse();
			}

			glm::ivec2 sizeOf(const std::string text) {
				glm::ivec2 res{ 0 };
				for (char c : text) {
					auto xter = character[c];
					res.x += (xter.advance >> 6);
					res.y  = glm::max(res.y, xter.size.y);
				}
				return res;
			}

			static void init(float w, float h) {
				if (FT_Init_FreeType(&library)) {
					std::string msg = "unable to load font library";
					logger.error(msg);
					throw std::runtime_error(msg);
				}
				width = w;
				height = h;
				sx = 2.0 / w;
				sy = 2.0 / h;

				location["Courier"] = "C:\\Windows\\Fonts\\cour.ttf";
				location["Arial"] = "C:\\Windows\\Fonts\\arial.ttf";
				location["Arial Bold"] = "C:\\Windows\\Fonts\\arialbd.ttf";
				location["Arial Bold Italic"] = "C:\\Windows\\Fonts\\arialbi.ttf";
				location["Arial Italic"] = " C:\\Windows\\Fonts\\ariali.ttf";

				// init shader;
				shader.loadFromstring(GL_VERTEX_SHADER, font_vert_shader);
				shader.loadFromstring(GL_FRAGMENT_SHADER, font_frag_shader);
				shader.createAndLinkProgram();

				glGenVertexArrays(1, &vaoId);
				glGenBuffers(1, &bufferId);
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

				withVertexArray(vaoId, [&]() {
					glBindBuffer(GL_ARRAY_BUFFER, bufferId);
					glBufferData(GL_ARRAY_BUFFER, sizeof(box), NULL, GL_DYNAMIC_DRAW);
					glEnableVertexAttribArray(0);
					glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
				});
			}

			void resize(int w, int h) {
				_width = w;
				_height = h;
				shader.use();
				projection = glm::ortho(0.0f, _width, 0.0f, _height);
				shader.sendUniformMatrix4fv("P", 1, GL_FALSE, glm::value_ptr(projection));
				shader.unUse();
			}

			glm::vec2 fontSize = glm::vec2(0);

		public:
			static std::map<std::string, std::string> location;
			static GLuint vaoId;
			static GLuint bufferId;
			static Shader shader;
			static Logger logger;
			static FT_Library library;
			static glm::vec4 box[4];
			static float sx, sy;
			static float width;
			static float height;
			static std::map<std::string, Font*> fonts;

			int style;
			int size;
			int maxHeight = std::numeric_limits<int>::min();
			int maxWidth = std::numeric_limits<int>::min();
			Character character[128];
			FT_Face face;
			glm::vec4 color;
			glm::mat4 projection;
			glm::vec4* background = nullptr;
			float _width;
			float _height;
			

			static std::string getFontName(std::string name, int style) {
				if (style & BOLD) {
					name += " Bold";
				}
				if (style & ITALIC) {
					name += " Italic";
				}
				return name;
			}
		};

		const int Font::BOLD = 0x01;
		const int Font::ITALIC = 0x02;
		std::map<std::string, std::string> Font::location;
		Shader Font::shader;
		Logger Font::logger = Logger::get("Font");
		GLuint Font::vaoId;
		GLuint Font::bufferId;
		FT_Library Font::library;
		glm::vec4 Font::box[4];
		float Font::sx;
		float Font::sy;
		float Font::width;
		float Font::height;
		std::map<std::string, Font*> Font::fonts;
	}
}