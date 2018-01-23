#pragma once
#include <iostream>


class vec3
{
public:
	vec3() : x(0.f), y(0.f), z(0.f) {}
	vec3(float in) : x(in), y(in), z(in) {}
	vec3(float x_in, float y_in, float z_in) : x(x_in), y(y_in), z(z_in) {}

	static float DotProduct(vec3 u, vec3 v) { return (u.x * v.x) + (u.y * v.y) + (u.z * v.z); }
	static vec3 CrossProduct(vec3 u, vec3 v) { return vec3(u.y * v.z - u.z * v.y, u.x * v.z - u.z * v.x, u.x * v.y - u.y * v.x); }
	static float Magnitude(const vec3& in) { return sqrtf((in.x * in.x) + (in.y * in.y) + (in.z * in.z)); }
	static vec3 Normalize(vec3& in) { return (in / Magnitude(in)); }

	friend std::ostream& operator<<(std::ostream& stream, const vec3& in)
	{
		stream << "vec3: " << in.x << ' ' << in.y << ' ' << in.z;
		return stream;
	}
	friend bool operator==(const vec3& lhs, const vec3& rhs)
	{
		return ((lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z)) ? true : false;
	}
	friend bool operator!=(const vec3& lhs, const vec3& rhs)
	{
		return ((lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z)) ? false : true;
	}
	friend vec3& operator+(vec3& lhs, const vec3& rhs)
	{
		lhs.x += rhs.x;
		lhs.y += rhs.y;
		lhs.z += rhs.z;

		return lhs;
	}
	friend vec3& operator-(vec3& lhs, const vec3& rhs)
	{
		lhs.x -= rhs.x;
		lhs.y -= rhs.y;
		lhs.z -= rhs.z;

		return lhs;
	}
	friend vec3& operator*(const float num, vec3& rhs)
	{
		rhs.x *= num;
		rhs.y *= num;
		rhs.z *= num;

		return rhs;
	}
	friend vec3& operator/(vec3& lhs, const float num)
	{
		lhs.x /= num;
		lhs.y /= num;
		lhs.z /= num;

		return lhs;
	}
	friend vec3& operator+=(vec3& lhs, const vec3& rhs) { return lhs = lhs + rhs; }
	friend vec3& operator-=(vec3& lhs, const vec3& rhs) { return lhs = lhs - rhs; }
	friend vec3& operator*=(vec3& lhs, const float num) { return lhs = num * lhs; }
	friend vec3& operator/=(vec3& lhs, const float num) { return lhs = lhs / num; }

public:
	float x, y, z;
};
