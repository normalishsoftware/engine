#pragma once
#include <GL\glew.h>
#include "NMath.h"
#include <iostream>

#define YAW -90.f
#define PITCH 0.f
#define SPEED 6.f
#define SENSITIVITY 0.25f
#define FOV 45.f


enum class CameraMovement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class Camera
{
public:
	glm::vec3 position;
	glm::vec3 up;
	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 world_up;

	float yaw;
	float pitch;
	float movement_speed;
	float mouse_sensitivity;
	float fov;

	Camera(glm::vec3 position = glm::vec3(0.f, 0.f, 0.f), glm::vec3 up = glm::vec3(0.f, 1.f, 0.f), float yaw = YAW, float pitch = PITCH)
		: front(glm::vec3(0.f, 0.f, -1.f)), movement_speed(SPEED), mouse_sensitivity(SENSITIVITY), fov(FOV)
	{
		this->position = position;
		this->world_up = up;
		this->yaw = yaw;
		this->pitch = pitch;
		this->UpdateCameraVectors();
	}

	Camera(float pos_x, float pos_y, float pos_z, float up_x, float up_y, float up_z, float yaw, float pitch)
		: front(glm::vec3(0.f, 0.f, -1.f)), movement_speed(SPEED), mouse_sensitivity(SENSITIVITY), fov(FOV)
	{
		this->position = glm::vec3(pos_x, pos_y, pos_z);
		this->world_up = glm::vec3(up_x, up_y, up_z);
		this->yaw = yaw;
		this->pitch = pitch;
		this->UpdateCameraVectors();
	}

	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(this->position, this->position + this->front, this->up);
	}

	void ProcessKeyboard(CameraMovement direction, float delta_time)
	{
		float velocity = this->movement_speed * delta_time;

		if (direction == CameraMovement::FORWARD)
			this->position += velocity * this->front;
		if (direction == CameraMovement::BACKWARD)
			this->position -= velocity * this->front;
		if (direction == CameraMovement::LEFT)
			this->position -= velocity * this->right;
		if (direction == CameraMovement::RIGHT)
			this->position += velocity * this->right;
	}

	void ProcessMouseMovement(float offset_x, float offset_y, bool clamp_pitch = true)
	{
		offset_x *= this->mouse_sensitivity;
		offset_y *= this->mouse_sensitivity;

		this->yaw += offset_x;
		this->pitch += offset_y;

		if (clamp_pitch)
		{
			if (this->pitch > 89.f)
				this->pitch = 89.f;
			if (this->pitch < -89.f)
				this->pitch = -89.f;
		}
		this->UpdateCameraVectors();
	}

	void UpdateCameraVectors()
	{
		glm::vec3 front;
		front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
		front.y = sin(glm::radians(this->pitch));
		front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));

		this->front = glm::normalize(front);
		this->right = glm::normalize(glm::cross(this->front, this->world_up));
		this->up = glm::normalize(glm::cross(this->right, this->front));
	}
};
