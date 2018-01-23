#pragma once
#include <cstdint>
#include <random>
#include <chrono>
#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "mat4.h"


class NMath
{
public:
	constexpr static float pi = 3.14159f;

	static const int32_t RoundDown(float in) { return static_cast<int32_t>(in); }
	static const int32_t RoundUp(float in) { return static_cast<int32_t>(in + 1); }

	static const float ToRad(float in) { return in * pi / 180.f; }
	static const float ToDeg(float in) { return in * 180.f / pi; }

	float Lerp(const float a, const float b, float alpha) { return (((1 - alpha) * a) + (alpha * b)); }
	vec3 Lerp(const vec3& u, const vec3& v, float alpha)
	{
		float beta = (1 - alpha);
		return vec3((beta * u.x) + (alpha * v.x), (beta * u.y) + (alpha * v.y), (beta * u.z) + (alpha * v.z));
	}
	vec4 Lerp(const vec4& u, const vec4& v, float alpha)
	{
		float beta = (1 - alpha);
		return vec4((beta * u.x) + (alpha * v.x), (beta * u.y) + (alpha * v.y), (beta * u.z) + (alpha * v.z), (beta * u.w) + (alpha * v.w));
	}

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
	static const vec2 RandomVec2(float lower, float upper)
	{
		return vec2(RandomFloat(lower, upper), RandomFloat(lower, upper));
	}
	static const vec3 RandomVec3(float lower, float upper)
	{
		return vec3(RandomFloat(lower, upper), RandomFloat(lower, upper), RandomFloat(lower, upper));
	}
	static const vec4 RandomVec4(float lower, float upper)
	{
		return vec4(RandomFloat(lower, upper), RandomFloat(lower, upper), RandomFloat(lower, upper), RandomFloat(lower, upper));
	}
};
