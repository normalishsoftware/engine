#pragma once
#include <iostream>
#include <cstdint>
#include "vec3.h"


class mat4
{
public:
	mat4()
	{
		for (auto& i : elements)
			i = 0.f;
	}
	mat4(float diagonal)
	{
		for (auto& i : elements)
			i = 0.f;

		elements[0] = diagonal;
		elements[5] = diagonal;
		elements[10] = diagonal;
		elements[15] = diagonal;
	}
	static mat4 identity() { return mat4(1.f); }
	static mat4 multiply(const mat4& lhs, const mat4& rhs)
	{
		mat4 ret;
		for (uint32_t rows = 0; rows < 4; rows++)
		{
			for (uint32_t cols = 0; cols < 4; cols++)
			{
				float sum = 0.f;
				for (uint32_t e = 0; e < 4; e++)
					sum += lhs.elements[cols + (e * 4)] * rhs.elements[e + (rows * 4)];

				ret.elements[rows + (cols * 4)] = sum;
			}
		}
		return ret;
	}
	static mat4& transpose(mat4& in)
	{
		float tmp = in.elements[1];
		in.elements[1] = in.elements[4];
		in.elements[4] = tmp;

		tmp = in.elements[2];
		in.elements[2] = in.elements[8];
		in.elements[8] = tmp;

		tmp = in.elements[3];
		in.elements[3] = in.elements[12];
		in.elements[12] = tmp;

		tmp = in.elements[6];
		in.elements[6] = in.elements[9];
		in.elements[9] = tmp;

		tmp = in.elements[7];
		in.elements[7] = in.elements[13];
		in.elements[13] = tmp;

		tmp = in.elements[11];
		in.elements[11] = in.elements[14];
		in.elements[14] = tmp;
	}
	static mat4 ortho(float left, float right, float bottom, float top, float near, float far)
	{
		mat4 ret;

		ret.elements[0] = 2.f / (right - left);
		ret.elements[5] = 2.f / (top - bottom);
		ret.elements[10] = 2.f / (near - far);
		ret.elements[12] = (right + left) / (right - left);
		ret.elements[13] = (top + bottom) / (top - bottom);
		ret.elements[14] = (far + near) / (far - near);
		ret.elements[15] = 1.f;

		return ret;
	}
	static mat4 persp(float fov, float aspect_ratio, float near, float far)
	{
		mat4 ret;

		float a = 1.f / tanf(0.5f * fov);
		float b = near - far;

		ret.elements[0] = a / aspect_ratio;
		ret.elements[5] = a;
		ret.elements[10] = (far + near) / b;
		ret.elements[11] = -1.f;
		ret.elements[14] = (2.f * far * near) / b;

		return ret;
	}
	static mat4 translate(mat4& original, const vec3& translation)
	{
		original.elements[12] += translation.x;
		original.elements[13] += translation.y;
		original.elements[14] += translation.z;

		return original;
	}
	static mat4 rotate(mat4& original, float angle, const vec3& axis)
	{
		float c = cosf(angle);
		float _c = 1.f - c;
		float s = sinf(angle);

		original.elements[0] = axis.x * axis.x * _c + c;
		original.elements[1] = axis.x * axis.y * _c + axis.z * s;
		original.elements[2] = axis.x * axis.z * _c - axis.y * s;
		original.elements[4] = axis.x * axis.y * _c - axis.z * s;
		original.elements[5] = axis.y * axis.y * _c + c;
		original.elements[6] = axis.y * axis.z * _c + axis.x * s;
		original.elements[8] = axis.x * axis.z * _c + axis.y * s;
		original.elements[9] = axis.y * axis.z * _c - axis.x * s;
		original.elements[10] = axis.z * axis.z * _c + c;
		original.elements[15] = 1.f;

		return original;
	}
	static mat4 scale(mat4& original, const vec3& scale)
	{
		original.elements[0] *= scale.x;
		original.elements[5] *= scale.y;
		original.elements[10] *= scale.z;
		original.elements[15] = 1.f;

		return original;
	}

	friend std::ostream& operator<<(std::ostream& stream, const mat4& in)
	{
		stream << "mat4:\n" << in.elements[0] << '\t' << in.elements[4] << '\t' << in.elements[8] << '\t' << in.elements[12] << '\n' << in.elements[1] << '\t' << in.elements[5] << '\t' << in.elements[9] << '\t' << in.elements[13] << '\n' << in.elements[2] << '\t' << in.elements[6] << '\t' << in.elements[10] << '\t' << in.elements[14] << '\n' << in.elements[3] << '\t' << in.elements[7] << '\t' << in.elements[11] << '\t' << in.elements[15];

		return stream;
	}
	friend mat4& operator+(mat4& lhs, const mat4& rhs)
	{
		for (int i = 0; i < 16; i++)
			lhs.elements[i] += rhs.elements[i];

		return lhs;
	}
	friend mat4& operator*(mat4& lhs, const mat4& rhs) { return lhs = mat4::multiply(lhs, rhs); }
	friend mat4& operator*=(mat4& lhs, const mat4& rhs) { return lhs = mat4::multiply(lhs, rhs); }
	friend bool operator==(const mat4& lhs, const mat4& rhs)
	{
		for (int i = 0; i < 16; i++)
		{
			if (lhs.elements[i] != rhs.elements[i])
				return false;
		}
		return true;
	}
	friend bool operator!=(const mat4& lhs, const mat4& rhs)
	{
		for (int i = 0; i < 16; i++)
		{
			if (lhs.elements[i] != rhs.elements[i])
				return true;
		}
		return false;
	}

public:
	float elements[16];
};
