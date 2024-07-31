#include "pch.h"
#include "models/entity/EntityModel.h"

namespace Jukcraft {

    constexpr float PIXEL_SIZE = 1.0f / 16.0f;


    Bone::Bone(const nlohmann::json& boneData, const glm::vec2& textureDim) {
        if (!boneData.empty()) {
            name = boneData["name"];
            pivot = glm::vec3(boneData["pivot"][0], boneData["pivot"][1], boneData["pivot"][2]) * PIXEL_SIZE;
            auto cubes = boneData["cubes"];
            for (const auto& cube : cubes) {
                float x = cube["origin"][0];
                float y = cube["origin"][1];
                float z = cube["origin"][2];
                auto rawPivot = cube.value("pivot", std::vector<float>({ 0.0f, 0.0f, 0.0f }));
                glm::vec3 pivot(rawPivot[0], rawPivot[1], rawPivot[2]);
                auto rawRot = cube.value("rotation", std::vector<float>({ 0.0f, 0.0f, 0.0f }));
                glm::vec3 rotation(rawRot[0], rawRot[1], rawRot[2]);
                float sx = cube["size"][0];
                float sy = cube["size"][1];
                float sz = cube["size"][2];
                float u = cube["uv"][0];
                float v = cube["uv"][1];

                rotation = glm::round(rotation / 90.0f) * 90.0f;

                glm::mat4 matrix = glm::translate(glm::mat4(1.0f), pivot);
                matrix = glm::rotate(matrix, -glm::radians(rotation[0]), glm::vec3(1, 0, 0));
                matrix = glm::rotate(matrix, -glm::radians(rotation[1]), glm::vec3(0, 1, 0));
                matrix = glm::rotate(matrix, -glm::radians(rotation[2]), glm::vec3(0, 0, 1));
                matrix = glm::translate(matrix, -pivot);

                auto transform = [&matrix](float x, float y, float z) {
                    return glm::vec3(matrix * glm::vec4(x, y, z, 1.0f)) * PIXEL_SIZE;
                };

                auto map_tex_coords = [&](const std::array<float, 8>& tex_coords)->std::array<glm::vec2, 4> {
                    std::array<glm::vec2, 4> mapped_coords;
                    for (size_t i = 0; i < tex_coords.size(); i += 2) {
                        float u = tex_coords[i] / textureDim.x;
                        float v = 1.0f - tex_coords[i + 1] / textureDim.y;
                        mapped_coords[i / 2] = glm::vec2(u, v);
                    }
                    return mapped_coords;
                };

                std::array<glm::vec3, 24> transformedVertices = {
                    transform(x, y, z), 
                    transform(x, y + sy, z), 
                    transform(x + sx, y + sy, z), 
                    transform(x + sx, y, z),

                    transform(x + sx, y, z + sz), 
                    transform(x + sx, y + sy, z + sz), 
                    transform(x, y + sy, z + sz), 
                    transform(x, y, z + sz),

                    transform(x, y + sy, z), 
                    transform(x, y + sy, z + sz), 
                    transform(x + sx, y + sy, z + sz), 
                    transform(x + sx, y + sy, z),

                    transform(x + sx, y, z), 
                    transform(x + sx, y, z + sz), 
                    transform(x, y, z + sz), 
                    transform(x, y, z),

                    transform(x + sx, y, z), 
                    transform(x + sx, y + sy, z), 
                    transform(x + sx, y + sy, z + sz), 
                    transform(x + sx, y, z + sz),
                        
                    transform(x, y, z + sz), 
                    transform(x, y + sy, z + sz), 
                    transform(x, y + sy, z), 
                    transform(x, y, z)
                };

                std::array<std::array<glm::vec2, 4>, 6> texCoords = {
                    map_tex_coords({u + sz, v + sz + sy, u + sz, v + sz, u + sz + sx, v + sz, u + sz + sx, v + sz + sy}),
                    map_tex_coords({u + sz + sx + sz, v + sz + sy, u + sz + sx + sz, v + sz, u + sz + sx + sz + sx, v + sz, u + sz + sx + sz + sx, v + sz + sy}),
                    map_tex_coords({u + sz, v + sz, u + sz, v, u + sz + sx, v, u + sz + sx, v + sz}),
                    map_tex_coords({u + sz + sx, v + sz, u + sz + sx, v, u + sz + sx + sx, v, u + sz + sx + sx, v + sz}),
                    map_tex_coords({u + sz + sx, v + sz + sy, u + sz + sx, v + sz, u + sz + sx + sz, v + sz, u + sz + sx + sz, v + sz + sy}),
                    map_tex_coords({u, v + sz + sy, u, v + sz, u + sz, v + sz, u + sz, v + sz + sy})
                };

                for (int i = 0; i < 6; ++i) {
                    for (int j = 0; j < 4; ++j) {
                        quads[i].vertices[j].pos = transformedVertices[i * 4 + j];
                        quads[i].vertices[j].texUV = texCoords[i][j];
                    }
                }
            }
        }
    }

    Bone::~Bone() {

    }





	EntityModel::EntityModel(const char* filename) {
        std::ifstream file(filename);
        nlohmann::json data;
        file >> data;

        glm::vec2 textureDim(data["texturewidth"], data["textureheight"]);
        auto raw_bones = data["bones"];


        for (const auto& raw_bone : raw_bones) {
            Bone bone(raw_bone, textureDim);         

            bones[bone.name] = bone;
        }
	}

    EntityModel::~EntityModel() {

    }
}