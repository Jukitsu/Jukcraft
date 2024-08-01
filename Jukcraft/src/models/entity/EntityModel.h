#pragma once

namespace Jukcraft {

    

    struct Bone {

        struct Vertex {
            glm::vec3 pos;
            glm::vec2 texUV;
        };

        struct Quad {
            std::array<Vertex, 4> vertices;
        };

        std::array<Quad, 6> quads;
        glm::vec3 pivot;
        std::string name;

        bool isLeg = false;
        bool isArm = false;
        bool isHead = false;
        bool isOdd = false;

        Bone() {}
        Bone(const nlohmann::json& boneData, const glm::vec2& textureDim);
        ~Bone();

    };

	struct EntityModel {
        EntityModel(const char* filepath);
        ~EntityModel();

        std::unordered_map<std::string, Bone> bones;
	};

    
}