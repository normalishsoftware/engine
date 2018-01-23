#pragma once
#include <iostream>


class vec4
{
public:
	vec4() : x(0.f), y(0.f), z(0.f), w(0.f) {}
	vec4(float in) : x(in), y(in), z(in), w(in) {}
	vec4(float x_in, float y_in, float z_in, float w_in) : x(x_in), y(y_in), z(z_in), w(w_in) {}

	// float DotProduct(vec3 u, vec3 v) { return (u.x * v.x) + (u.y * v.y) + (u.z * v.z); }
	// vec3 CrossProduct(vec3 u, vec3 v) { return vec3(u.y * v.z - u.z * v.y, u.x * v.z - u.z * v.x, u.x * v.y - u.y * v.x); }
	static float Magnitude(const vec4& in) { return sqrtf((in.x * in.x) + (in.y * in.y) + (in.z * in.z) + (in.w * in.w)); }
	static vec4 Normalize(vec4& in) { return (in / Magnitude(in)); }

	friend std::ostream& operator<<(std::ostream& stream, const vec4& in)
	{
		stream << "vec4: " << in.x << ' ' << in.y << ' ' << in.z << ' ' << in.w;
		return stream;
	}
	friend bool operator==(const vec4& lhs, const vec4& rhs)
	{
		return ((lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z) && (lhs.w == rhs.w)) ? true : false;
	}
	friend bool operator!=(const vec4& lhs, const vec4& rhs)
	{
		return ((lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z) && (lhs.w == rhs.w)) ? false : true;
	}
	friend vec4& operator+(vec4& lhs, const vec4& rhs)
	{
		lhs.x += rhs.x;
		lhs.y += rhs.y;
		lhs.z += rhs.z;
		lhs.w += rhs.w;

		return lhs;
	}
	friend vec4& operator-(vec4& lhs, const vec4& rhs)
	{
		lhs.x -= rhs.x;
		lhs.y -= rhs.y;
		lhs.z -= rhs.z;
		lhs.w -= rhs.w;

		return lhs;
	}
	friend vec4& operator*(const float num, vec4& rhs)
	{
		rhs.x *= num;
		rhs.y *= num;
		rhs.z *= num;
		rhs.w *= num;

		return rhs;
	}
	friend vec4& operator/(vec4& lhs, const float num)
	{
		lhs.x /= num;
		lhs.y /= num;
		lhs.z /= num;
		lhs.w /= num;

		return lhs;
	}
	friend vec4& operator+=(vec4& lhs, const vec4& rhs) { return lhs = lhs + rhs; }
	friend vec4& operator-=(vec4& lhs, const vec4& rhs) { return lhs = lhs - rhs; }
	friend vec4& operator*=(vec4& lhs, const float num) { return lhs = num * lhs; }
	friend vec4& operator/=(vec4& lhs, const float num) { return lhs = lhs / num; }

public:
	float x, y, z, w;
};
