#pragma once

#include "GLAD/glad.h"
#include "GLFW/glfw3.h"
#include "helpers.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "glObjects/VAO.h"
#include "glObjects/VBO.h"
#include "glObjects/EBO.h"
#include "glObjects/Camera.h"
#include "ResourceManager.h"
#include "Cube.h"
#include "Chunk.h"

enum GameState {
	GAME_ACTIVE = 0,
	GAME_MENU,
};

class Game
{
public:
	Game(float width, float heigth);
	Game(const Game&) = delete;
	Game(Game&&) = delete;
	Game& operator=(const Game&) = delete;
	Game& operator=(Game&&) = delete;

	~Game();

	void run();

private:
	void processInput(float dt);
	void update(float dt);
	void render();

private:
	void framebuffer_size_callback(int width, int height);
	void mouse_move_callback(double xpos, double ypos);
	void mouse_scroll_callback(double xoffset, double yoffset);

	static void static_framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void static_mouse_move_callback(GLFWwindow* window, double xpos, double ypos);
	static void static_mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

private:
	// Game state.
	GameState State = GameState::GAME_ACTIVE;
	bool Keys[1024];

	// Window.
	GLFWwindow* window;
	float Width = 800.0f;
	float Height = 600.0f;

	// Camera.
	Camera camera;
	float lastCursorX = Width / 2;
	float lastCursorY = Height / 2;
	bool isFirstMouseInput = true;

	// Game Objects.
	std::vector<Cube> Cubes;
	std::vector<Chunk> Chunks;
};
