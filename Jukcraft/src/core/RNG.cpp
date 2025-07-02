#include "pch.h"


namespace Jukcraft {

	static std::optional<std::mt19937> rng;


	void initRandom() {
		rng.emplace(std::random_device()());
	}

	int randomDiscrete(int a, int b) {
		std::uniform_int_distribution<int> dist(a, b);
		return dist(*rng);
	}
	float randomContinuous(float a, float b) {
		std::uniform_real_distribution<float> dist(a, b);
		return dist(*rng);
	}
}