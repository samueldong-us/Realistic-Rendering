#include <cstdio>
#include <cstdlib>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "glitter.hpp"

#include "Renderer.hpp"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

AdvancedRenderer::Renderer renderer;

int main(int argc, char * argv[])
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	auto mWindow = glfwCreateWindow(mWidth, mHeight, "OpenGL", nullptr, nullptr);

	if (mWindow == nullptr) {
		fprintf(stderr, "Failed to Create OpenGL Context");
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(mWindow);
	gladLoadGL();
	fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

	glfwSetKeyCallback(mWindow, key_callback);
	glfwSetCursorPosCallback(mWindow, mouse_callback);
	glfwSetScrollCallback(mWindow, scroll_callback);

	renderer.InitializeOpenGL();
	renderer.LoadContent();

	double currentTime = -1.0;
	while (glfwWindowShouldClose(mWindow) == false) {
		glfwPollEvents();
		if (currentTime != -1.0)
		{
			renderer.Render();
			renderer.Update(glfwGetTime() - currentTime);
			glfwSwapBuffers(mWindow);
		}
		currentTime = glfwGetTime();
	}
	glfwTerminate();
	return EXIT_SUCCESS;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (action == GLFW_PRESS)
		renderer.OnKeyPressed(key);
	else if (action == GLFW_RELEASE)
		renderer.OnKeyReleased(key);
}

bool leftButtonDown = false;
bool rightButtonDown = false;
bool initialized = false;
double lastX, lastY;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	int leftButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	int rightButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);

	if (!leftButtonDown && leftButtonState == GLFW_PRESS)
		renderer.OnMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT);
	if (leftButtonDown && leftButtonState == GLFW_RELEASE)
		renderer.OnMouseButtonReleased(GLFW_MOUSE_BUTTON_LEFT);
	if (!rightButtonDown && rightButtonState == GLFW_PRESS)
		renderer.OnMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT);
	if (rightButtonDown && rightButtonState == GLFW_RELEASE)
		renderer.OnMouseButtonReleased(GLFW_MOUSE_BUTTON_RIGHT);

	leftButtonDown = leftButtonState == GLFW_PRESS;
	rightButtonDown = rightButtonState == GLFW_PRESS;

	if (initialized)
	{
		renderer.OnMouseMoved(xpos - lastX, lastY - ypos);
	}
	lastX = xpos;
	lastY = ypos;
	initialized = true;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	renderer.OnMouseScrolled(yoffset);
}