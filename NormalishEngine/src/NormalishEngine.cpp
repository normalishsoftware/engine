#include <iostream>
#include <cstdint>
#include <chrono>
#include <fstream>
#include <sstream>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <SOIL\SOIL2.h>
#include <fmod.hpp>
#include "Core\NMath.h"
#include "Debug\Debug.h"
#include "Core\Utils.h"
#include "Core\Graphics\Shader.h"
#include "AudioManager.h"
#include "Core\Camera.h"


#define SALT "Normalish"
#define SIZE_OF_SALT 10

std::chrono::duration<float> DeltaTime;
Debug* debug;
Camera camera(vec3(0.f, 0.f, -10.f));
float last_x = 800.f / 2.f;
float last_y = 600.f / 2.f;
float first_mouse = true;
bool keys[1024];

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_PRESS)
		Debug::HotReload();
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	if (first_mouse)
	{
		last_x = xPos;
		last_y = yPos;
		first_mouse = false;
	}

	float offset_x = xPos - last_x;
	float offset_y = last_y - yPos;

	last_x = xPos;
	last_y = yPos;

	camera.ProcessMouseMovement(offset_x, offset_y);
}

void DoMovement()
{
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(CameraMovement::FORWARD, DeltaTime.count());
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(CameraMovement::BACKWARD, DeltaTime.count());
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(CameraMovement::LEFT, DeltaTime.count());
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(CameraMovement::RIGHT, DeltaTime.count());
}

int32_t main()
{
	ConfigData Config;

	LoadConfigs(Config);

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
		window = glfwCreateWindow(Config.resolution_x, Config.resolution_y, "Engine", nullptr, nullptr);
	else
		window = glfwCreateWindow(Config.resolution_x, Config.resolution_y, "Engine", glfwGetPrimaryMonitor(), nullptr);

	if (!window)
	{
		std::cout << "Failed to create window\n";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(Config.vsync);
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
	Shader::LoadShader(".\\res\\basic.shader", &vertex_shader, &fragment_shader);
	uint32_t shader = Shader::CreateShader(vertex_shader.c_str(), fragment_shader.c_str());
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

	vec3 cube_positions[] =
	{
		vec3(0.f),
		vec3(2.f, 5.f, -15.f),
		vec3(-1.5f, -2.1f, -2.5f),
		vec3(-3.f, 2.f, -12.f),
		vec3(4.f)
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

	unsigned char* image = SOIL_load_image(".\\res\\wood.jpg", &tex_width, &tex_height, nullptr, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	AudioManager* audio_system = new AudioManager();
	audio_system->Init();

	//audio_system->PlaySound(".\\res\\music.wav", false);

	while (!glfwWindowShouldClose(window))
	{
		auto frame_start = std::chrono::high_resolution_clock::now();
		glfwPollEvents();
		DoMovement();

		glClearColor(0.6f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(glGetUniformLocation(shader, "_texture"), 0);

		glUseProgram(shader);

		mat4 projection = mat4::persp(NMath::pi / 4.f, (float)Config.resolution_x / (float)Config.resolution_y, 0.1f, 1000.f);

		mat4 model(1.f);
		mat4 view(1.f);
		view = camera.GetViewMatrix();

		int32_t model_location = glGetUniformLocation(shader, "model");
		int32_t view_location = glGetUniformLocation(shader, "view");
		int32_t projection_location = glGetUniformLocation(shader, "projection");

		glUniformMatrix4fv(view_location, 1, GL_FALSE, view.elements);
		glUniformMatrix4fv(projection_location, 1, GL_FALSE, projection.elements);

		glBindVertexArray(vertex_array);
		for (uint32_t i = 0; i < 4; i++)
		{
			mat4 model(1.f);
			model = mat4::translate(model, cube_positions[i]);
			float angle = 20.f * i;
			model = mat4::rotate(model, angle, vec3(1.f, 0.3f, 0.5f));
			glUniformMatrix4fv(model_location, 1, GL_FALSE, model.elements);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);

		glfwSwapBuffers(window);
		auto frame_end = std::chrono::high_resolution_clock::now();
		DeltaTime = frame_end - frame_start;
	}

	glDeleteVertexArrays(1, &vertex_array);
	glDeleteBuffers(1, &vertex_buffer);
	audio_system->Terminate();
	delete audio_system;
	glfwTerminate();
	return 0;
}
