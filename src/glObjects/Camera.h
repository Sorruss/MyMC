#pragma once

#include "glm/gtc/matrix_transform.hpp"
#include "GLFW/glfw3.h"

class Camera
{
public:
	Camera();
	Camera(const Camera&) = delete;
	Camera(Camera&&) = delete;
	Camera& operator=(const Camera&) = delete;
	Camera& operator=(Camera&&) = delete;

	void Init(glm::vec3& up);
	void processInput(GLFWwindow* window, float dt);
	void calculateMatrix();
	
	void updateCursorOffset(float xOffset, float yOffset);
	void updateFOV(double yoffset);
	void updateSpeed(double yoffset);

	glm::vec3 getDirectionView() const;

private:
	float speed = 2.5f;
	float sensitivity = 0.1f;

	float yaw = -90.0f;
	float pitch = 0.0f;

	glm::vec3 worldUp;

public:
	glm::mat4 view;
	float fov = 45.0f;
	glm::vec3 pos;
	glm::vec3 front;
};
