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
Camera camera(glm::vec3(0.f, 0.f, 3.f));
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

	float offset_x = (float)xPos - last_x;
	float offset_y = last_y - (float)yPos;

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

glm::vec3 light_position(1.2f, 1.f, 2.f);

int32_t main()
{
	ConfigData Config;

	LoadConfigs(Config);

	Debug::AddHot(string_hash("camera.position", 16, SALT, SIZE_OF_SALT), &camera.position);

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
	Shader::LoadShader(".\\res\\lighting.shader", &vertex_shader, &fragment_shader);
	uint32_t lighting_shader = Shader::CreateShader(vertex_shader.c_str(), fragment_shader.c_str());
	Shader::LoadShader(".\\res\\lamp.shader", &vertex_shader, &fragment_shader);
	uint32_t lamp_shader = Shader::CreateShader(vertex_shader.c_str(), fragment_shader.c_str());

	float vertices[] =
	{
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	uint32_t cube_vertex_array, vertex_buffer;
	glGenVertexArrays(1, &cube_vertex_array);
	glGenBuffers(1, &vertex_buffer);

	glBindVertexArray(cube_vertex_array);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// vertex positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	// vertex normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	uint32_t light_vertex_array;
	glGenVertexArrays(1, &light_vertex_array);
	glGenBuffers(1, &vertex_buffer);

	glBindVertexArray(light_vertex_array);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	AudioManager* audio_system = new AudioManager();
	audio_system->Init();

	//audio_system->PlaySound(".\\res\\music.wav", false);

	glm::mat4 projection = glm::perspective(camera.fov, (float)Config.resolution_x / (float)Config.resolution_y, 0.1f, 1000.f);

	while (!glfwWindowShouldClose(window))
	{
		auto frame_start = std::chrono::high_resolution_clock::now();
		glfwPollEvents();
		DoMovement();

		glClearColor(0.1f, 0.1f, 0.1f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(lighting_shader);
		int32_t light_position_location = glGetUniformLocation(lighting_shader, "light.position");
		int32_t view_position_location = glGetUniformLocation(lighting_shader, "view_position");
		glUniform3f(light_position_location, light_position.x, light_position.y, light_position.z);
		glUniform3f(view_position_location, camera.position.x, camera.position.y, camera.position.z);

		glm::vec3 light_color;
		light_color.r = sin(glfwGetTime() * 2.f);
		light_color.g = sin(glfwGetTime() * 0.7f);
		light_color.b = sin(glfwGetTime() * 1.3f);

		glm::vec3 diffuse_color = light_color * glm::vec3(0.5f);
		glm::vec3 ambient_color = diffuse_color * glm::vec3(0.2f);
		glUniform3f(glGetUniformLocation(lighting_shader, "light.ambient"), ambient_color.r, ambient_color.g, ambient_color.b);
		glUniform3f(glGetUniformLocation(lighting_shader, "light.diffuse"), diffuse_color.r, diffuse_color.g, diffuse_color.b);
		glUniform3f(glGetUniformLocation(lighting_shader, "light.specular"), 1.f, 1.f, 1.f);

		glUniform3f(glGetUniformLocation(lighting_shader, "material.ambient"), 1.f, 0.5f, 0.31f);
		glUniform3f(glGetUniformLocation(lighting_shader, "material.diffuse"), 1.f, 0.5f, 0.31f);
		glUniform3f(glGetUniformLocation(lighting_shader, "material.specular"), 0.5f, 0.5f, 0.5f);
		glUniform1f(glGetUniformLocation(lighting_shader, "material.shininess"), 32.f);

		glm::mat4 view;
		view = camera.GetViewMatrix();
		int32_t model_location = glGetUniformLocation(lighting_shader, "model");
		int32_t view_location = glGetUniformLocation(lighting_shader, "view");
		int32_t projection_location = glGetUniformLocation(lighting_shader, "projection");

		glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(cube_vertex_array);
		glm::mat4 model;
		glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		glUseProgram(lamp_shader);

		model_location = glGetUniformLocation(lamp_shader, "model");
		view_location = glGetUniformLocation(lamp_shader, "view");
		projection_location = glGetUniformLocation(lamp_shader, "projection");

		glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::mat4();
		model = glm::translate(model, light_position);
		model = glm::scale(model, glm::vec3(0.2));
		glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(light_vertex_array);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
		auto frame_end = std::chrono::high_resolution_clock::now();
		DeltaTime = frame_end - frame_start;
	}

	glDeleteVertexArrays(1, &cube_vertex_array);
	glDeleteVertexArrays(1, &light_vertex_array);
	glDeleteBuffers(1, &vertex_buffer);
	audio_system->Terminate();
	delete audio_system;
	glfwTerminate();
	return 0;
}
