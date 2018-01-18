#include <iostream>
#include <cstdint>
#include <random>
#include <chrono>
#include <fstream>
#include <sstream>
#include <string>
#include <tuple>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <SOIL\SOIL2.h>
#include <fmod.hpp>

#define SALT "Normalish"
#define SIZE_OF_SALT 10

std::vector<std::tuple<uint32_t, int32_t*>> hot_reload_ints(5);
std::vector<std::tuple<uint32_t, float*>> hot_reload_floats(5);
std::vector<std::tuple<uint32_t, bool*>> hot_reload_bools(5);

std::chrono::duration<float> DeltaTime;

struct ConfigData
{
	uint32_t resolution_x;
	uint32_t resolution_y;
	uint32_t fullscreen;
	uint32_t vsync;

	ConfigData() : resolution_x(640), resolution_y(480), fullscreen(0), vsync(0) {}
};

ConfigData Config;

template<class InputIterator, class T>
bool bool_find(InputIterator first, InputIterator last, const T& val)
{
	while (first != last)
	{
		if (*first == val)
			return true;

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

class math
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
void AddVar(uint32_t name_hash, bool *var_ptr) { hot_reload_bools.push_back(std::make_tuple(name_hash, var_ptr)); }

void HotReload()
{
	std::ifstream file("hotreload.txt");

	if (file.is_open())
	{
		std::string line;
		while (std::getline(file, line))
		{
			uint32_t space = line.find(' ');
			uint32_t name_hash = string_hash(line.substr(0, space).c_str(), space, SALT, SIZE_OF_SALT);
			std::string value = line.substr(space);

			for (auto i : hot_reload_floats)
			{
				if (std::get<0>(i) == name_hash)
					*std::get<1>(i) = std::stof(value);
			}
			for (auto j : hot_reload_ints)
			{
				if (std::get<0>(j) == name_hash)
					*std::get<1>(j) = std::stoi(value);
			}
			for (auto k : hot_reload_bools)
			{
				if (std::get<0>(k) == name_hash)
					*std::get<1>(k) = static_cast<bool>(std::stoi(value));
			}
		}
	}
	else
	{
		std::cout << "Error could not find file\n";
	}
}

void LoadConfigs()
{
	std::ifstream file("config.txt");

	uint32_t resolution_x = string_hash("resolution_x", 13, SALT, SIZE_OF_SALT);
	uint32_t resolution_y = string_hash("resolution_y", 13, SALT, SIZE_OF_SALT);
	uint32_t fullscreen = string_hash("fullscreen", 11, SALT, SIZE_OF_SALT);
	uint32_t vsync = string_hash("vsync", 6, SALT, SIZE_OF_SALT);

	if (file.is_open())
	{
		std::string line;
		while (std::getline(file, line))
		{
			uint32_t space = line.find(' ');
			uint32_t name_hash = string_hash(line.substr(0, space).c_str(), space, SALT, SIZE_OF_SALT);
			std::string value = line.substr(space);

			if (name_hash == resolution_x)
				Config.resolution_x = std::stoi(value);
			if (name_hash == resolution_y)
				Config.resolution_y = std::stoi(value);
			if (name_hash == fullscreen)
				Config.fullscreen = std::stoi(value);
			if (name_hash == vsync)
				Config.vsync = std::stoi(value);
		}
	}
}

void LoadShader(const char* file_path, std::string* vertex_source, std::string* fragment_source)
{
	enum class ShaderType
	{
		NONE = -1,
		VERTEX,
		FRAGMENT
	};

	std::ifstream file(file_path);
	std::stringstream ss[2];
	ShaderType shader_type = ShaderType::NONE;

	if (file.is_open())
	{
		std::string line;
		while (std::getline(file, line))
		{
			if (line.find("#shader") != std::string::npos)
			{
				if (line.find("vertex") != std::string::npos)
					shader_type = ShaderType::VERTEX;
				else if (line.find("fragment") != std::string::npos)
					shader_type = ShaderType::FRAGMENT;
			}
			else
				ss[static_cast<int32_t>(shader_type)] << line << '\n';
		}
	}
	*vertex_source = ss[0].str();
	*fragment_source = ss[1].str();
}

constexpr uint32_t CompileShader(uint32_t type, const char* source)
{
	uint32_t id = glCreateShader(type);
	glShaderSource(id, 1, &source, nullptr);
	glCompileShader(id);

	{
		int32_t result = 0;
		glGetShaderiv(id, GL_COMPILE_STATUS, &result);
		if (!result)
		{
			int32_t length = 0;
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

constexpr uint32_t CreateShader(const char* vertex_shader, const char* fragment_shader)
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
	LoadConfigs();

	if (!glfwInit())
	{
		std::cout << "Failed to init GLFW\n";
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window;
	if (Config.fullscreen == 0)
		window = glfwCreateWindow(Config.resolution_x, Config.resolution_y, "Normalish", nullptr, nullptr);
	else
		window = glfwCreateWindow(Config.resolution_x, Config.resolution_y, "Normalish", glfwGetPrimaryMonitor(), nullptr);

	if (!window)
	{
		std::cout << "Failed to create window\n";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(Config.vsync);

	glewExperimental = true;

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to init GLEW\n";
		return -1;
	}

	glViewport(0, 0, Config.resolution_x, Config.resolution_y);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	std::string vertex_shader, fragment_shader;
	LoadShader("basic.shader", &vertex_shader, &fragment_shader);
	uint32_t shader = CreateShader(vertex_shader.c_str(), fragment_shader.c_str());
	glUseProgram(shader);

	float vertices[] =
	{
		-0.5f, -0.5f, -0.5f,	0.f, 0.f,
		0.5f, -0.5f, -0.5f,		1.f, 0.f,
		0.5f, 0.5f, -0.5f,		1.f, 1.f,
		0.5f, 0.5f, -0.5f,		1.f, 1.f,
		-0.5f, 0.5f, -0.5f,		0.f, 1.f,
		-0.5f, -0.5f, -0.5f,	0.f, 0.f,

		-0.5f, -0.5f, 0.5f,		0.f, 0.f,
		0.5f, -0.5f, 0.5f,		1.f, 0.f,
		0.5f, 0.5f, 0.5f,		1.f, 1.f,
		0.5f, 0.5f, 0.5f,		1.f, 1.f,
		-0.5f, 0.5f, 0.5f,		0.f, 1.f,
		-0.5f, -0.5f, 0.5f,		0.f, 0.f,

		-0.5f, 0.5f, 0.5f,		1.f, 0.f,
		-0.5f, 0.5f, -0.5f,		1.f, 1.f,
		-0.5f, -0.5f, -0.5f,	0.f, 1.f,
		-0.5f, -0.5f, -0.5f,	0.f, 1.f,
		-0.5f, -0.5f, 0.5f,		0.f, 0.f,
		-0.5f, 0.5f, 0.5f,		1.f, 0.f,

		0.5f, 0.5f, 0.5f,		1.f, 0.f,
		0.5f, 0.5f, -0.5f,		1.f, 1.f,
		0.5f, -0.5f, -0.5f,		0.f, 1.f,
		0.5f, -0.5f, -0.5f,		0.f, 1.f,
		0.5f, -0.5f, 0.5f,		0.f, 0.f,
		0.5f, 0.5f, 0.5f,		1.f, 0.f,

		-0.5f, -0.5f, -0.5f,	0.f, 1.f,
		0.5f, -0.5f, -0.5f,		1.f, 1.f,
		0.5f, -0.5f, 0.5f,		1.f, 0.f,
		0.5f, -0.5f, 0.5f,		1.f, 0.f,
		-0.5f, -0.5f, 0.5f,		0.f, 0.f,
		-0.5f, -0.5f, -0.5f,	0.f, 1.f,

		-0.5f, 0.5f, -0.5f,		0.f, 1.f,
		0.5f, 0.5f, -0.5f,		1.f, 1.f,
		0.5f, 0.5f, 0.5f,		1.f, 0.f,
		0.5f, 0.5f, 0.5f,		1.f, 0.f,
		-0.5f, 0.5f, 0.5f,		0.f, 0.f,
		-0.5f, 0.5f, -0.5f,		0.f, 1.f
	};

	uint32_t vertex_array, vertex_buffer;
	glGenVertexArrays(1, &vertex_array);
	glGenBuffers(1, &vertex_buffer);

	glBindVertexArray(vertex_array);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	uint32_t texture;
	int32_t tex_width, tex_height;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned char* image = SOIL_load_image("wood.jpg", &tex_width, &tex_height, nullptr, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	mat4 projection(1.f);
	projection = mat4::persp(math::pi / 4.f, (float)Config.resolution_x / (float)Config.resolution_y, 0.1f, 1000.f);

	FMOD::System *system;
	FMOD::System_Create(&system);
	FMOD_RESULT result = system->init(100, FMOD_INIT_NORMAL, nullptr);
	if (result != FMOD_OK)
	{
		std::cout << "Failed to initialize FMOD\n";
		return -1;
	}

	FMOD::Sound *stream;
	system->createStream("music.wav", FMOD_DEFAULT | FMOD_LOOP_NORMAL, nullptr, &stream);

	FMOD::Channel *channel;
	system->playSound(stream, nullptr, false, &channel);

	while (!glfwWindowShouldClose(window))
	{
		auto frame_start = std::chrono::high_resolution_clock::now();
		glfwPollEvents();

		system->update();

		glClearColor(0.6f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(glGetUniformLocation(shader, "_texture"), 0);

		glUseProgram(shader);

		mat4 model(1.f);
		mat4 view(1.f);
		model = mat4::rotate(model, glfwGetTime(), vec3(0.5f, 1.f, 0.f));
		view = mat4::translate(view, vec3(0.f, 0.f, -3.f));

		int32_t model_location = glGetUniformLocation(shader, "model");
		int32_t view_location = glGetUniformLocation(shader, "view");
		int32_t projection_location = glGetUniformLocation(shader, "projection");

		glUniformMatrix4fv(model_location, 1, GL_FALSE, model.elements);
		glUniformMatrix4fv(view_location, 1, GL_FALSE, view.elements);
		glUniformMatrix4fv(projection_location, 1, GL_FALSE, projection.elements);

		glBindVertexArray(vertex_array);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
		auto frame_end = std::chrono::high_resolution_clock::now();
		DeltaTime = frame_end - frame_start;
	}

	glDeleteVertexArrays(1, &vertex_array);
	glDeleteBuffers(1, &vertex_buffer);
	stream->release();
	system->release();
	glfwTerminate();
	return 0;
}
