#pragma once
#include <iostream>


class vec2
{
public:
	vec2() : x(0.f), y(0.f) {}
	vec2(float in) : x(in), y(in) {}
	vec2(float x_in, float y_in) : x(x_in), y(y_in) {}

	static float DotProduct(vec2 u, vec2 v) { return (u.x * v.x) + (u.y * v.y); }
	static float Magnitude(const vec2& in) { return sqrtf((in.x * in.x) + (in.y * in.y)); }
	static vec2 Normalize(vec2& in) { return (in / Magnitude(in)); }

	friend std::ostream& operator<<(std::ostream& stream, const vec2& in)
	{
		stream << "vec2: " << in.x << ' ' << in.y;
		return stream;
	}
	friend bool operator==(const vec2& lhs, const vec2& rhs)
	{
		return ((lhs.x == rhs.x) && (lhs.y == rhs.y)) ? true : false;
	}
	friend bool operator!=(const vec2& lhs, const vec2& rhs)
	{
		return ((lhs.x == rhs.x) && (lhs.y == rhs.y)) ? false : true;
	}
	friend vec2& operator+(vec2& lhs, const vec2& rhs)
	{
		lhs.x += rhs.x;
		lhs.y += rhs.y;

		return lhs;
	}
	friend vec2& operator-(vec2& lhs, const vec2& rhs)
	{
		lhs.x -= rhs.x;
		lhs.y -= rhs.y;

		return lhs;
	}
	friend vec2& operator*(const float num, vec2& rhs)
	{
		rhs.x *= num;
		rhs.y *= num;

		return rhs;
	}
	friend vec2& operator/(vec2& lhs, const float num)
	{
		lhs.x /= num;
		lhs.y /= num;

		return lhs;
	}
	friend vec2& operator+=(vec2& lhs, const vec2& rhs) { return lhs = lhs + rhs; }
	friend vec2& operator-=(vec2& lhs, const vec2& rhs) { return lhs = lhs - rhs; }
	friend vec2& operator*=(vec2& lhs, const float num) { return lhs = num * lhs; }
	friend vec2& operator/=(vec2& lhs, const float num) { return lhs = lhs / num; }

public:
	float x, y;
};
