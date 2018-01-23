#pragma once
#include <GL\glew.h>
#include "NMath.h"
#include <iostream>

#define YAW -90.f
#define PITCH 0.f
#define SPEED 60.f
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
	vec3 position;
	vec3 up;
	vec3 front;
	vec3 right;
	vec3 world_up;

	float yaw;
	float pitch;
	float movement_speed;
	float mouse_sensitivity;
	float fov;

	Camera(vec3 position = vec3(0.f), vec3 up = vec3(0.f, 1.f, 0.f), float yaw = YAW, float pitch = PITCH)
		: front(vec3(0.f, 0.f, -1.f)), movement_speed(SPEED), mouse_sensitivity(SENSITIVITY), fov(FOV)
	{
		this->position = position;
		this->world_up = up;
		this->yaw = yaw;
		this->pitch = pitch;
		this->UpdateCameraVectors();
	}

	Camera(float pos_x, float pos_y, float pos_z, float up_x, float up_y, float up_z, float yaw, float pitch)
		: front(vec3(0.f, 0.f, -1.f)), movement_speed(SPEED), mouse_sensitivity(SENSITIVITY), fov(FOV)
	{
		this->position = vec3(pos_x, pos_y, pos_z);
		this->world_up = vec3(up_x, up_y, up_z);
		this->yaw = yaw;
		this->pitch = pitch;
		this->UpdateCameraVectors();
	}

	mat4 GetViewMatrix()
	{
		vec3 const f(vec3::Normalize(this->position - this->front));
		vec3 const s(vec3::Normalize(vec3::CrossProduct(f, this->up)));
		vec3 const u(vec3::CrossProduct(s, f));

		mat4 ret(1.f);
		ret.elements[0] = s.x;
		ret.elements[1] = s.y;
		ret.elements[2] = s.z;
		ret.elements[4] = u.x;
		ret.elements[5] = u.y;
		ret.elements[6] = u.z;
		ret.elements[8] = -f.x;
		ret.elements[9] = -f.y;
		ret.elements[10] = -f.z;
		ret.elements[3] = -vec3::DotProduct(s, this->front);
		ret.elements[7] = -vec3::DotProduct(u, this->front);
		ret.elements[11] = vec3::DotProduct(f, this->front);

		return ret;
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
		vec3 front;
		front.x = cos(NMath::ToRad(this->yaw)) * cos(NMath::ToRad(this->pitch));
		front.y = sin(NMath::ToRad(this->pitch));
		front.z = sin(NMath::ToRad(this->pitch)) * cos(NMath::ToRad(this->pitch));

		this->front = vec3::Normalize(front);
		this->right = vec3::Normalize(vec3::CrossProduct(this->front, this->world_up));
		this->up = vec3::Normalize(vec3::CrossProduct(this->right, this->front));
	}
};
