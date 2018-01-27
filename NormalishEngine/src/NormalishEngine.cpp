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
		// Positions            // Normals              // Texture Coords
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,  	1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,

		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f
	};

	glm::vec3 cube_positions[] =
	{
		glm::vec3(0.0f,   0.0f,   0.0f),
		glm::vec3(2.0f,   5.0f,   -15.0f),
		glm::vec3(-1.5f,  -2.2f,  -2.5f),
		glm::vec3(-3.8f,  -2.0f,  -12.3f),
		glm::vec3(2.4f,   -0.4f,  -3.5f),
		glm::vec3(-1.7f,  3.0f,   -7.5f),
		glm::vec3(1.3f,   -2.0f,  -2.5f),
		glm::vec3(1.5f,   2.0f,   -2.5f),
		glm::vec3(1.5f,   0.2f,   -1.5f),
		glm::vec3(-1.3f,  1.0f,   -1.5f)
	};

	glm::vec3 point_light_positions[] = 
	{
		glm::vec3(0.7f, 0.2f, 2.f),
		glm::vec3(2.3f, -3.3f, -4.f),
		glm::vec3(-4.f, 2.f, -12.f),
		glm::vec3(0.f, 0.f, -3.f)
	};

	uint32_t cube_vertex_array, vertex_buffer;
	glGenVertexArrays(1, &cube_vertex_array);
	glGenBuffers(1, &vertex_buffer);

	glBindVertexArray(cube_vertex_array);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// vertex positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	// vertex normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// texture coordinates
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	uint32_t light_vertex_array;
	glGenVertexArrays(1, &light_vertex_array);
	glGenBuffers(1, &vertex_buffer);

	glBindVertexArray(light_vertex_array);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	uint32_t diffuse_map, specular_map, emission_map;
	glGenTextures(1, &diffuse_map);
	glGenTextures(1, &specular_map);
	glGenTextures(1, &emission_map);

	int32_t texture_width, texture_height;
	unsigned char* image;

	//diffuse map
	image = SOIL_load_image(".\\res\\iron-rusted4-basecolor.png", &texture_width, &texture_height, 0, SOIL_LOAD_RGB);
	glBindTexture(GL_TEXTURE_2D, diffuse_map);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);

	//specular map
	image = SOIL_load_image(".\\res\\iron-rusted4-metalness.png", &texture_width, &texture_height, 0, SOIL_LOAD_RGB);
	glBindTexture(GL_TEXTURE_2D, specular_map);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(lighting_shader);
	glUniform1i(glGetUniformLocation(lighting_shader, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(lighting_shader, "material.specular"), 1);

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
		int32_t view_position_location = glGetUniformLocation(lighting_shader, "view_position");
		glUniform3f(view_position_location, camera.position.x, camera.position.y, camera.position.z);
		glUniform1f(glGetUniformLocation(lighting_shader, "material.shininess"), 32.f);

		//directional light
		glUniform3f(glGetUniformLocation(lighting_shader, "directional_light.direction"), -0.2f, -1.f, -0.3f);
		glUniform3f(glGetUniformLocation(lighting_shader, "directional_light.ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lighting_shader, "directional_light.diffuse"), 0.4f, 0.4f, 0.4f);
		glUniform3f(glGetUniformLocation(lighting_shader, "directional_light.specular"), 0.5f, 0.5f, 0.5f);

		//point light 0
		glUniform3f(glGetUniformLocation(lighting_shader, "point_lights[0].position"), point_light_positions[0].x, point_light_positions[0].y, point_light_positions[0].z);
		glUniform3f(glGetUniformLocation(lighting_shader, "point_lights[0].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lighting_shader, "point_lights[0].diffuse"), 0.8f, 0.8f, 0.8f);
		glUniform3f(glGetUniformLocation(lighting_shader, "point_lights[0].specular"), 1.f, 1.f, 1.f);
		glUniform1f(glGetUniformLocation(lighting_shader, "point_lights[0].constant"), 1.f);
		glUniform1f(glGetUniformLocation(lighting_shader, "point_lights[0].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lighting_shader, "point_lights[0].quadratic"), 0.032f);

		//point light 1
		glUniform3f(glGetUniformLocation(lighting_shader, "point_lights[1].position"), point_light_positions[1].x, point_light_positions[1].y, point_light_positions[1].z);
		glUniform3f(glGetUniformLocation(lighting_shader, "point_lights[1].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lighting_shader, "point_lights[1].diffuse"), 0.8f, 0.8f, 0.8f);
		glUniform3f(glGetUniformLocation(lighting_shader, "point_lights[1].specular"), 1.f, 1.f, 1.f);
		glUniform1f(glGetUniformLocation(lighting_shader, "point_lights[1].constant"), 1.f);
		glUniform1f(glGetUniformLocation(lighting_shader, "point_lights[1].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lighting_shader, "point_lights[1].quadratic"), 0.032f);

		//point light 2
		glUniform3f(glGetUniformLocation(lighting_shader, "point_lights[2].position"), point_light_positions[2].x, point_light_positions[2].y, point_light_positions[2].z);
		glUniform3f(glGetUniformLocation(lighting_shader, "point_lights[2].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lighting_shader, "point_lights[2].diffuse"), 0.8f, 0.8f, 0.8f);
		glUniform3f(glGetUniformLocation(lighting_shader, "point_lights[2].specular"), 1.f, 1.f, 1.f);
		glUniform1f(glGetUniformLocation(lighting_shader, "point_lights[2].constant"), 1.f);
		glUniform1f(glGetUniformLocation(lighting_shader, "point_lights[2].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lighting_shader, "point_lights[2].quadratic"), 0.032f);

		//point light 3
		glUniform3f(glGetUniformLocation(lighting_shader, "point_lights[3].position"), point_light_positions[3].x, point_light_positions[3].y, point_light_positions[3].z);
		glUniform3f(glGetUniformLocation(lighting_shader, "point_lights[3].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lighting_shader, "point_lights[3].diffuse"), 0.8f, 0.8f, 0.8f);
		glUniform3f(glGetUniformLocation(lighting_shader, "point_lights[3].specular"), 1.f, 1.f, 1.f);
		glUniform1f(glGetUniformLocation(lighting_shader, "point_lights[3].constant"), 1.f);
		glUniform1f(glGetUniformLocation(lighting_shader, "point_lights[3].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lighting_shader, "point_lights[3].quadratic"), 0.032f);

		//spot light
		glUniform3f(glGetUniformLocation(lighting_shader, "spot_light.position"), camera.position.x, camera.position.y, camera.position.z);
		glUniform3f(glGetUniformLocation(lighting_shader, "spot_light.direction"), camera.front.x, camera.front.y, camera.front.z);
		glUniform3f(glGetUniformLocation(lighting_shader, "spot_light.ambient"), 0.f, 0.f, 0.f);
		glUniform3f(glGetUniformLocation(lighting_shader, "spot_light.diffuse"), 1.f, 1.f, 1.f);
		glUniform3f(glGetUniformLocation(lighting_shader, "spot_light.specular"), 1.f, 1.f, 1.f);
		glUniform1f(glGetUniformLocation(lighting_shader, "spot_light.constant"), 1.f);
		glUniform1f(glGetUniformLocation(lighting_shader, "spot_light.linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lighting_shader, "spot_light.quadratic"), 0.032f);
		glUniform1f(glGetUniformLocation(lighting_shader, "spot_light.cutoff"), cos(glm::radians(12.5f)));
		glUniform1f(glGetUniformLocation(lighting_shader, "spot_light.outer_cutoff"), cos(glm::radians(15.f)));

		glm::mat4 view;
		view = camera.GetViewMatrix();

		int32_t model_location = glGetUniformLocation(lighting_shader, "model");
		int32_t view_location = glGetUniformLocation(lighting_shader, "view");
		int32_t projection_location = glGetUniformLocation(lighting_shader, "projection");

		glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(projection));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuse_map);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specular_map);

		glm::mat4 model;
		glBindVertexArray(cube_vertex_array);

		for (uint32_t i = 0; i < 10; i++)
		{
			model = glm::mat4();
			model = glm::translate(model, cube_positions[i]);
			float angle = 20.f * i;
			model = glm::rotate(model, angle, glm::vec3(1.f, 0.3f, 0.5f));
			glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
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

		glBindVertexArray(light_vertex_array);
		for (uint32_t i = 0; i < 4; i++)
		{
			model = glm::mat4();
			model = glm::translate(model, point_light_positions[i]);
			model = glm::scale(model, glm::vec3(0.2));
			glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
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
