#include "Game.h"

Game::Game(float width, float height) 
	: Width(width), Height(height), Keys()
{
	// GLFW settings
	// ---------------------------------------------------------------
	// Initializing GLFW and setting window hints for proper behaviour.
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Initializing window context and window itself.
	if (window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), "MyMC", 0, 0); window == NULL)
	{
		PrintError("glfw window/context couldn't be created.");
	}
	glfwMakeContextCurrent(window);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, static_framebuffer_size_callback);

	// GLAD
	// -----------------
	// Initializing GLAD.
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		PrintError("glad couldn't be loaded.");
	}

	// OpenGL
	// ----------------------------------------
	// Telling OpenGL which resolution we using.
	glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Depth buffer enable.
	glEnable(GL_DEPTH_TEST);

	// Telling OpenGL about face meshing thingy.
	glFrontFace(GL_CW);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// My things
	// ------------------------
	// Initializing the shaders.
	ResourceManager::LoadShader("default", "Resources/Shaders/default.vertex", "Resources/Shaders/default.fragment");

	// Initializing the camera.
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, static_mouse_move_callback);
	glfwSetScrollCallback(window, static_mouse_scroll_callback);
	glm::vec3 up{ 0.0f, 1.0f, 0.0f };
	camera.Init(up);

	// Loading atlases.
	ResourceManager::LoadTexture("atlas-1", "Resources/Textures/atlas_terrain.png", 0);
}

Game::~Game()
{
	glfwTerminate();
}

void Game::run()
{
	ShaderProgram& defaultShader = ResourceManager::GetShader("default");
	Chunks.emplace_back(glm::vec2(0.0f, 0.0f));

	float lastDT = 0.0f;
	while (!glfwWindowShouldClose(window))
	{
		// General.
		float time = static_cast<float>(glfwGetTime());
		float deltaTime = time - lastDT;
		lastDT = time;

		processInput(deltaTime);
		camera.calculateMatrix();

		glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		render();
		glfwSwapBuffers(window); // Swapping front and back buffers.
		glfwPollEvents(); // Polling instructed user events so glfw can call respective callback functions to handle them.
	}

	ResourceManager::Clear();
	glfwTerminate();
}

void Game::processInput(float dt)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) // ESCAPE - Close.
	{
		glfwSetWindowShouldClose(window, true);
	}

	camera.processInput(window, dt);
}

void Game::update(float dt)
{

}

void Game::render()
{
	glm::mat4 projection = glm::perspective(glm::radians(camera.fov), Width / Height, 0.1f, 100.0f);
	for (Cube& cube : Cubes)
	{
		cube.Draw(ResourceManager::GetShader("default"), camera, projection);
	}

	for (Chunk& chunk : Chunks)
	{
		chunk.Render(ResourceManager::GetShader("default"), camera, projection);
	}
}

void Game::framebuffer_size_callback(int width, int height)
{
	this->Width = static_cast<float>(width);
	this->Height = static_cast<float>(height);
	glViewport(0, 0, width, height);
}

void Game::mouse_move_callback(double xpos, double ypos)
{
	float xposf = static_cast<float>(xpos);
	float yposf = static_cast<float>(ypos);

	if (isFirstMouseInput)
	{
		lastCursorX = xposf;
		lastCursorY = yposf;
		isFirstMouseInput = false;
	}

	float xOffset = xposf - lastCursorX;
	float yOffset = lastCursorY - yposf;
	lastCursorX = xposf;
	lastCursorY = yposf;

	camera.updateCursorOffset(xOffset, yOffset);
}

void Game::mouse_scroll_callback(double xoffset, double yoffset)
{
	//camera.updateFOV(yoffset);
	camera.updateSpeed(yoffset);
}

void Game::static_framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
	if (game)
	{
		game->framebuffer_size_callback(width, height);
	}
}

void Game::static_mouse_move_callback(GLFWwindow* window, double xpos, double ypos)
{
	Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
	if (game)
	{
		game->mouse_move_callback(xpos, ypos);
	}
}

void Game::static_mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
	if (game)
	{
		game->mouse_scroll_callback(xoffset, yoffset);
	}
}
