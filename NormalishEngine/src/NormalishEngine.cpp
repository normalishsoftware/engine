#include <iostream>
#include <cstdint>
#include <random>
#include <chrono>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <fstream>
#include <string>
#include <tuple>
#include <typeinfo>

#define SALT "Normalish"
#define SIZE_OF_SALT 10

std::vector<std::tuple<uint32_t, int32_t*>> hot_reload_ints(5);
std::vector<std::tuple<uint32_t, float*>> hot_reload_floats(5);
std::vector<std::tuple<uint32_t, bool*>> hot_reload_bools(5);

template<class InputIterator, class T>
bool bool_find(InputIterator first, InputIterator last, const T& val)
{
	while (first != last) {
		if (*first == val) return true;
		++first;
	}
	return false;
}

class vec2
{
public:
	vec2() : x(0.f), y(0.f) {}
	vec2(float in) : x(in), y(in) {}
	vec2(float x_in, float y_in) : x(x_in), y(y_in) {}

	float DotProduct(vec2 u, vec2 v) { return (u.x * v.x) + (u.y * v.y); }

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
	friend vec2& operator/(const float num, vec2& rhs)
	{
		rhs.x /= num;
		rhs.y /= num;

		return rhs;
	}
	friend vec2& operator+=(vec2& lhs, const vec2& rhs) { return lhs = lhs + rhs; }
	friend vec2& operator-=(vec2& lhs, const vec2& rhs) { return lhs = lhs - rhs; }
	friend vec2& operator*=(vec2& lhs, const float num) { return lhs = num * lhs; }
	friend vec2& operator/=(vec2& lhs, const float num) { return lhs = num / lhs; }

public:
	float x, y;
};

class vec3
{
public:
	vec3() : x(0.f), y(0.f), z(0.f) {}
	vec3(float in) : x(in), y(in), z(in) {}
	vec3(float x_in, float y_in, float z_in) : x(x_in), y(y_in), z(z_in) {}

	float DotProduct(vec3 u, vec3 v) { return (u.x * v.x) + (u.y * v.y) + (u.z * v.z); }
	vec3 CrossProduct(vec3 u, vec3 v) { return vec3(u.y * v.z - u.z * v.y, u.x * v.z - u.z * v.x, u.x * v.y - u.y * v.x); }
	float Magnitude(const vec3& in) { return sqrtf((in.x * in.x) + (in.y * in.y) + (in.z * in.z)); }
	
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
	friend vec3& operator/(const float num, vec3& rhs)
	{
		rhs.x /= num;
		rhs.y /= num;
		rhs.z /= num;

		return rhs;
	}
	friend vec3& operator+=(vec3& lhs, const vec3& rhs) { return lhs = lhs + rhs; }
	friend vec3& operator-=(vec3& lhs, const vec3& rhs) { return lhs = lhs - rhs; }
	friend vec3& operator*=(vec3& lhs, const float num) { return lhs = num * lhs; }
	friend vec3& operator/=(vec3& lhs, const float num) { return lhs = num / lhs; }

public:
	float x, y, z;
};

class vec4
{
public:
	vec4() : x(0.f), y(0.f), z(0.f), w(0.f) {}
	vec4(float in) : x(in), y(in), z(in), w(in) {}
	vec4(float x_in, float y_in, float z_in, float w_in) : x(x_in), y(y_in), z(z_in), w(w_in) {}

	// float DotProduct(vec3 u, vec3 v) { return (u.x * v.x) + (u.y * v.y) + (u.z * v.z); }
	// vec3 CrossProduct(vec3 u, vec3 v) { return vec3(u.y * v.z - u.z * v.y, u.x * v.z - u.z * v.x, u.x * v.y - u.y * v.x); }

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
	friend vec4& operator/(const float num, vec4& rhs)
	{
		rhs.x /= num;
		rhs.y /= num;
		rhs.z /= num;
		rhs.w /= num;

		return rhs;
	}
	friend vec4& operator+=(vec4& lhs, const vec4& rhs) { return lhs = lhs + rhs; }
	friend vec4& operator-=(vec4& lhs, const vec4& rhs) { return lhs = lhs - rhs; }
	friend vec4& operator*=(vec4& lhs, const float num) { return lhs = num * lhs; }
	friend vec4& operator/=(vec4& lhs, const float num) { return lhs = num / lhs; }

public:
	float x, y, z, w;
};

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
				{
					sum += lhs.elements[cols + (e * 4)] * rhs.elements[e + (rows * 4)];
				}
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
	mat4 ortho(float left, float right, float bottom, float top, float near, float far)
	{
		mat4 ret;

		ret.elements[0] = 2 / (right - left);
		ret.elements[5] = 2 / (top - bottom);
		ret.elements[10] = 2 / (near - far);
		ret.elements[12] = (right + left) / (right - left);
		ret.elements[13] = (top + bottom) / (top - bottom);
		ret.elements[14] = (far + near) / (far - near);
		ret.elements[15] = 1;

		return ret;
	}
	mat4 persp(float fov, float aspect_ratio, float near, float far)
	{
		mat4 ret;

		float a = 1 / tanf(fov / 2);
		float b = near - far;

		ret.elements[0] = a / aspect_ratio;
		ret.elements[5] = a;
		ret.elements[10] = (far + near) / b;
		ret.elements[11] = -1;
		ret.elements[14] = (2 * far * near) / b;

		return ret;
	}
	mat4 translation(const vec3& translation)
	{
		mat4 ret(1.f);

		ret.elements[12] = translation.x;
		ret.elements[13] = translation.y;
		ret.elements[14] = translation.z;

		return ret;
	}
	mat4 rotation(float angle, const vec3& axis)
	{
		mat4 ret(1.f);

		float c = cosf(angle);
		float _c = 1 - c;
		float s = sinf(angle);

		ret.elements[0] = (axis.x * axis.x * _c) + c;
		ret.elements[1] = (axis.x * axis.y * _c) + (axis.z * s);
		ret.elements[2] = (axis.x * axis.z * _c) + (axis.y * s);
		ret.elements[4] = (axis.x * axis.y * _c) - (axis.z * s);
		ret.elements[5] = (axis.y * axis.y * _c) + c;
		ret.elements[6] = (axis.y * axis.z * _c) + (axis.x * s);
		ret.elements[8] = (axis.x * axis.z * _c) + (axis.y * s);
		ret.elements[9] = (axis.y * axis.z * _c) - (axis.x * s);
		ret.elements[10] = (axis.z * axis.z * _c) + c;

		return ret;
	}
	mat4 scale(const vec3& scale)
	{
		mat4 ret(1.f);

		ret.elements[0] = scale.x;
		ret.elements[5] = scale.y;
		ret.elements[10] = scale.z;

		return ret;
	}

	friend std::ostream& operator<<(std::ostream& stream, const mat4& in)
	{
		stream << "mat4:\n" << in.elements[0] << '\t' << in.elements[4] << '\t' << in.elements[8] << '\t' << in.elements[12] << '\n' << in.elements[1] << '\t' << in.elements[5] << '\t' << in.elements[9] << '\t' << in.elements[13] << '\n' << in.elements[2] << '\t' << in.elements[6] << '\t' << in.elements[10] << '\t' << in.elements[14] << '\n' << in.elements[3] << '\t' << in.elements[7] << '\t' << in.elements[11] << '\t' << in.elements[15];

		return stream;
	}
	friend mat4& operator+(mat4& lhs, const mat4& rhs)
	{
		for (int i = 0; i < 16; i++)
		{
			lhs.elements[i] += rhs.elements[i];
		}
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

class math
{
public:
	constexpr static float pi = 3.14159f;

	static const int32_t RoundDown(float in) { return static_cast<int32_t>(in); }
	static const int32_t RoundUp(float in) { return static_cast<int32_t>(in + 1); }

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

uint32_t string_hash(const char* data, uint32_t data_length, const char* salt, uint32_t salt_length)
{
	uint32_t hash = 0;
	unsigned char* hash_ptr = reinterpret_cast<unsigned char*>(&hash);

	for (uint32_t i = 0; i < salt_length; i++)
		hash_ptr[i % 4] = hash_ptr[i % 4] ^ salt[i];

	for (uint32_t j = 0; j < data_length; j++)
		hash_ptr[j % 4] = hash_ptr[j % 4] ^ data[j];

	return hash;
}

void AddVar(uint32_t name_hash, float *var_ptr) { hot_reload_floats.push_back(std::make_tuple(name_hash, var_ptr)); }
void AddVar(uint32_t name_hash, int32_t *var_ptr) { hot_reload_ints.push_back(std::make_tuple(name_hash, var_ptr)); }

void HotReload()
{
	std::string line;
	std::ifstream file("hotreload.txt");
	if (file.is_open())
	{
		while (std::getline(file, line))
		{
			uint32_t space = line.find(' ');
			uint32_t name_hash = string_hash(line.substr(0, space).c_str(), space, SALT, SIZE_OF_SALT);
			std::string value = line.substr(space);

			for (auto i : hot_reload_floats)
			{
				if (std::get<0>(i) == name_hash)
				{
					*std::get<1>(i) = std::stof(value);
					std::cout << *std::get<1>(i) << '\n';
				}
			}
			for (auto j : hot_reload_ints)
			{
				if (std::get<0>(j) == name_hash)
				{
					*std::get<1>(j) = std::stoi(value);
					std::cout << *std::get<1>(j) << '\n';
				}
			}
			for (auto k : hot_reload_bools)
			{
				if (std::get<0>(k) == name_hash)
				{
					*std::get<1>(k) = static_cast<bool>(std::stoi(value));
					std::cout << *std::get<1>(k) << '\n';
				}
			}
		}
	}
	else
	{
		std::cout << "Error could not find file\n";
	}
}

static uint32_t CompileShader(uint32_t type, const char* source)
{
	uint32_t id = glCreateShader(type);
	glShaderSource(id, 1, &source, nullptr);
	glCompileShader(id);

	{
		int32_t result;
		glGetShaderiv(id, GL_COMPILE_STATUS, &result);
		if (!result)
		{
			int32_t length;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
			char* error = static_cast<char*>(alloca(length * sizeof(char)));
			glGetShaderInfoLog(id, length, &length, error);
			std::cout << error << '\n';
			glDeleteShader(id);
			return 0;
		}
	}

	return id;
}

static uint32_t CreateShader(const char* vertex_shader, const char* fragment_shader)
{
	uint32_t program = glCreateProgram();
	uint32_t vs = CompileShader(GL_VERTEX_SHADER, vertex_shader);
	uint32_t fs = CompileShader(GL_FRAGMENT_SHADER, fragment_shader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	else if (key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_PRESS)
		HotReload();
}

int32_t main()
{
	int32_t health = 100;
	AddVar(string_hash("health", 7, SALT, SIZE_OF_SALT), &health);

	float stamina = 32.2f;
	AddVar(string_hash("stamina", 8, SALT, SIZE_OF_SALT), &stamina);

	int32_t ammo = 100;
	AddVar(string_hash("ammo", 5, SALT, SIZE_OF_SALT), &ammo);

	if (!glfwInit())
	{
		return -1;
	}

	GLFWwindow* window;
	window = glfwCreateWindow(640, 480, "Normalish", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);

	if (glewInit() != GLEW_OK)
	{
		return -1;
	}

	float positions[] = { -0.5f, -0.5f,
						   0.f, 0.5f,
						   0.5f, -0.5f };

	uint32_t vertex_buffer;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);

	const char* vertex_shader =
		"#version 330 core\n"
		"layout(location = 0) in vec4 position;\n"
		"void main()\n"
		"{\n"
		"gl_Position = position;\n"
		"}";
	const char* fragment_shader =
		"#version 330 core\n"
		"layout(location = 0) out vec4 color;\n"
		"void main()\n"
		"{\n"
		"color = vec4(0.7f, 0.f, 1.f, 1.f);\n"
		"}";
	uint32_t shader = CreateShader(vertex_shader, fragment_shader);
	glUseProgram(shader);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
	}

	glDeleteProgram(shader);

	glfwTerminate();
	return 0;
}
