#pragma once


#include <map>
#include <string>
#include <vector>
#include <stdexcept>
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

			static Font* getFont(std::string name, int size, int style, const color& color) {
				auto key = getFontName(name, style) + std::to_string(size);
				auto itr = fonts.find(key);
				if (itr == fonts.end()) {
					fonts[key] = new Font(name, size, style, color);
				}
				return fonts[key];
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

			void render(const std::string& text, float x, float y) {
				shader.use();
				projection = glm::ortho(0.0f, width, 0.0f, height);
				shader.sendUniformMatrix4fv("P", 1, GL_FALSE, glm::value_ptr(projection));
				if (background) {
					shader.send("useBackgroundColor", true);
					shader.sendUniform1fv("backgroundColor", 1, &(*background)[0]);
					delete background;
					background = nullptr;
				}
				withVertexArray(vaoId, [&]() {

					bool blendingOff = !glIsEnabled(GL_BLEND);
					if (blendingOff) glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					
					for (char c : text) {
						auto ch = character[c];
						
						glActiveTexture(GL_TEXTURE10);
						glBindTexture(GL_TEXTURE_2D, ch.id);
						shader.sendUniform1ui("glyph", 10);

						shader.sendUniform4fv("color", 1, glm::value_ptr(color));

						float x2 = x + ch.bearing.x;
						float y2 = y - (ch.size.y - ch.bearing.y);
						float w = ch.size.x;
						float h = ch.size.y;

						using namespace glm;

						box[0] = { x2, y2 + h    , 0, 0 };
						box[1] = { x2, y2, 0, 1 };
						box[2] = { x2 + w, y2 + h, 1, 0 };
						box[3] = { x2 + w, y2    , 1, 1 };

						glm::vec4* buffer = (glm::vec4*)glMapNamedBuffer(bufferId, GL_READ_WRITE);

						buffer[0] = box[0];
						buffer[1] = box[1];
						buffer[2] = box[2];
						buffer[3] = box[3];

						glUnmapNamedBuffer(bufferId);
						glPointSize(5);
						glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

						x += (ch.advance >> 6);
					}
					if (blendingOff) glDisable(GL_BLEND);
					
				});
				shader.unUse();
			}

			glm::ivec2 sizeOf(const std::string text) {
				glm::ivec2 res{ 0 };
				for (char c : text) {
					auto xter = character[c];
					res.x += xter.size.x;
					res.y += xter.size.y;
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

		private:
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
			Character character[128];
			FT_Face face;
			glm::vec4 color;
			glm::mat4 projection;
			glm::vec4* background = nullptr;
			

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