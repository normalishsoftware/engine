#pragma once
#include <cstdint>
#include <random>
#include <chrono>
#include "glm\vec2.hpp"
#include "glm\vec3.hpp"
#include "glm\vec4.hpp"
#include "glm\mat4x4.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "glm\gtc\constants.hpp"


class NMath
{
public:
	static const int32_t RoundDown(float in) { return static_cast<int32_t>(in); }
	static const int32_t RoundUp(float in) { return static_cast<int32_t>(in + 1); }

	static const int32_t RandomInt(int32_t lower, int32_t upper)
	{
		std::mt19937 generator(std::chrono::system_clock::now().time_since_epoch().count());
		std::uniform_int_distribution<int32_t> distribution(lower, upper);
		return distribution(generator);
	}
	static const float RandomFloat(float lower, float upper)
	{
		std::mt19937 generator(std::chrono::system_clock::now().time_since_epoch().count());
		std::uniform_real_distribution<float> distribution(lower, upper);
		return distribution(generator);
	}
	static const glm::vec2 RandomVec2(float lower, float upper)
	{
		return glm::vec2(RandomFloat(lower, upper), RandomFloat(lower, upper));
	}
	static const glm::vec3 RandomVec3(float lower, float upper)
	{
		return glm::vec3(RandomFloat(lower, upper), RandomFloat(lower, upper), RandomFloat(lower, upper));
	}
	static const glm::vec4 RandomVec4(float lower, float upper)
	{
		return glm::vec4(RandomFloat(lower, upper), RandomFloat(lower, upper), RandomFloat(lower, upper), RandomFloat(lower, upper));
	}
};
