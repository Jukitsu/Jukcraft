#pragma once
#include "renderer/models/Model.h"

class Block {
public:
	Block(const char*, BlockID id, Model& model, const std::vector<uint8_t>& textures, bool transparent, uint8_t light) 
		:id(id), model(model), textures(textures), transparent(transparent), light(light) {
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
	constexpr uint8_t getLight() const {
		return light;
	}
private:
	BlockID id;
	Model& model;
	std::vector<uint8_t> textures;
	bool transparent;
	uint8_t light;
};