#pragma once

#include <vector>
#include <variant>
#include <algorithm>

namespace ncl {
	namespace gl {

		//Texture2DArray::Texture2DArray(Config conf, GLuint unit) 
		//	:config{ conf } {
		//	auto contents = std::vector<Data>(conf.contents.size());
		//	std::transform(conf.contents.begin(), conf.contents.end(), contents.begin(), [&](auto content) {
		//		return std::visit(TextureVisitor{conf.type}, content);
		//	});

		//	glGenTextures(1, &_buffer);
		//	glBindTexture(GL_TEXTURE_2D_ARRAY, _buffer);
		//	
		//	for (int layer = 0; layer < contents.size(); layer++) {
		//		auto content = contents[layer];
		//		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, conf.internalFmt, content.width, content.height, layer, 0, conf.format, conf.type, content.contents);
		//	}

		//	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, conf.minFilter);
		//	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, conf.magFilter);
		//	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, conf.wrapS);
		//	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, conf.wrapT);
		////	glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, conf.borderColor);
		//	if (config.mipMap) glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
		//}
	}
}