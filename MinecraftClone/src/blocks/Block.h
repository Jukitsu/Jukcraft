#pragma once
#include "renderer/models/Model.h"

class Block {
public:
	Block(const char*, BlockID id, Model& model, const std::vector<uint8_t>& textures, bool transparent) 
		:id(id), model(model), textures(textures), transparent(transparent) {
		CHECK(textures.size() == model.getQuads().size(), "Texture layout does not match model");
	}
	bool isCube() const {
		return model.getQuads().size() == 6;
	}
	constexpr const Model& getModel() const {
		return model;
	}
	constexpr const std::vector<uint8_t>& getTextureLayout() const {
		return textures;
	}
	constexpr bool isTransparent() const {
		return transparent;
	}
private:
	BlockID id;
	Model& model;
	std::vector<uint8_t> textures;
	bool transparent;
};