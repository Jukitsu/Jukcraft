#pragma once
#include <stb_image.h>

class TextureManager {
public:
	TextureManager(uint16_t dim) :dim(dim), index(0) {
		glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &handle);
		glTextureStorage3D(handle, static_cast<int>(std::floor(std::log2f(dim))) + 1, GL_RGBA8, dim, dim, 256);
	}
	~TextureManager() {
		glDeleteTextures(1, &handle);
	}
	void pushSubTexture(const std::string_view& path) {
		stbi_set_flip_vertically_on_load(true);
		int x, y, channels;
		stbi_uc* pixels = stbi_load(path.data(), &x, &y, &channels, 0);
		GLenum format;
		if (channels == 3)
			format = GL_RGB;
		else if (channels == 4)
			format = GL_RGBA;
		else
			THROW_ERROR("Wrong file format");
		glTextureSubImage3D(handle, 0, 0, 0, index, x, y, 1, format, GL_UNSIGNED_BYTE, pixels);
		index++;
		stbi_image_free(pixels);
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
	uint16_t dim;
	uint8_t index;
};