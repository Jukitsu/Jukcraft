#pragma once
#include <stb_image.h>

namespace Jukcraft {
	class Texture {
	public:
		Texture(const std::string_view& path) :index(0) {
			stbi_set_flip_vertically_on_load(true);
			int x, y, channels;
			stbi_uc* pixels = stbi_load(path.data(), &x, &y, &channels, 0);
			GLenum format;
			if (channels == 3) // RGB channels
				format = GL_RGB;
			else if (channels == 4) // RGBA channels
				format = GL_RGBA;
			else
				THROW_ERROR("Wrong file format");

			glCreateTextures(GL_TEXTURE_2D, 1, &handle);
			glTextureStorage2D(handle, 4, GL_RGBA8, x, y);
			
			glTextureSubImage2D(handle, 0, 0, 0, x, y, format, GL_UNSIGNED_BYTE, pixels);
			stbi_image_free(pixels);
			width = x;
			height = y;
		}
		~Texture() {
			glDeleteTextures(1, &handle);
		}

		void setSamplerUnit(uint8_t unit) {
			glTextureParameteri(handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTextureParameteri(handle, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTextureParameteri(handle, GL_TEXTURE_MAX_ANISOTROPY, 8);

			glGenerateTextureMipmap(handle);
			glBindTextureUnit(unit, handle);
		}
	private:
		GLuint handle;
		uint16_t width, height;
		uint8_t index;
	};
}