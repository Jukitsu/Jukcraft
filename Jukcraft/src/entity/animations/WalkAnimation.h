#pragma once


namespace Jukcraft {
    struct WalkAnimation {
        float speedOld = 0.0f;
        float speed = 0.0f;
        float position = 0.0f;

        void update(float newSpeed, float g) {
            speedOld = speed;
            speed += (newSpeed - speed) * g;
            position += speed;
        }

        float lerpSpeed(float partialTicks) const {
            return glm::mix(speedOld, speed, partialTicks);
        }

        float lerpPos(float partialTicks) const {
            return position - speed * (1.0F - partialTicks);
        }

        bool isMoving() const {
            return speed > 1.0E-5F;
        }


    };
}