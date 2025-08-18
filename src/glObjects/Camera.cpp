#include "Camera.h"

Camera::Camera()
{
	worldUp = glm::vec3(0.0f);
	pos = glm::vec3(0.0f);
	front = glm::vec3(0.0f);
	view = glm::mat4(1.0f);
}

void Camera::Init(glm::vec3& up)
{
	worldUp = up;
	pos = { 0.0f, 0.0f, 4.0f };
	front = { 0.0f, 0.0f, -1.0f };
}

void Camera::processInput(GLFWwindow* window, float dt)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		pos += front * speed * dt;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		pos -= front * speed * dt;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		pos -= glm::normalize(glm::cross(front, worldUp)) * speed * dt;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		pos += glm::normalize(glm::cross(front, worldUp)) * speed * dt;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		pos += worldUp * speed * dt;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		pos -= worldUp * speed * dt;
	}
}

void Camera::calculateMatrix()
{
	view = glm::lookAt(pos, pos + front, worldUp);
}

void Camera::updateCursorOffset(float xOffset, float yOffset)
{
	xOffset *= sensitivity;
	yOffset *= sensitivity;

	yaw += xOffset;
	pitch += yOffset;

	if (pitch > 89.0f)
	{
		pitch = 89.0f;
	}
	else if (pitch < -89.0f)
	{
		pitch = -89.0f;
	}

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(direction);
}

void Camera::updateFOV(double yoffset)
{
	fov -= static_cast<float>(yoffset);
	if (fov > 45.0f)
	{
		fov = 45.0f;
	}
	else if (fov < 1.0f)
	{
		fov = 1.0f;
	}
}

void Camera::updateSpeed(double yoffset)
{
	speed += static_cast<float>(yoffset);
	if (speed > 10.0f)
	{
		speed = 10.0f;
	}
	else if (speed < 1.0f)
	{
		speed = 1.0f;
	}
}

glm::vec3 Camera::getDirectionView() const
{
	return view * glm::vec4(pos + front, 1.0f);
}
