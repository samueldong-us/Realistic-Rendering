#include <memory>

#include "glad\glad.h"
#include "glm\glm.hpp"
#include "GLFW\glfw3.h"

#include "filesystem.hpp"
#include "camera.hpp"
#include "model.hpp"

#include "InitialGBufferStage.h"

#include "Renderer.hpp"

using namespace std;
using namespace glm;

namespace AdvancedRenderer
{
	void Renderer::InitializeOpenGL()
	{
		glEnable(GL_DEPTH_TEST);
	}

	void Renderer::LoadContent()
	{
		initialGBufferStage = unique_ptr<InitialGBufferStage>(new InitialGBufferStage());

		camera = unique_ptr<Camera>(new Camera(vec3(0.0f, 0.0f, 2.0f)));
		model = unique_ptr<Model>(new Model(FileSystem::getPath("Resources/crytek_sponza/sponza.obj")));
	}

	void Renderer::Update(const float deltaTime)
	{
		if (IsKeyDown(GLFW_KEY_W))
			camera->ProcessKeyboard(FORWARD, deltaTime);
		if (IsKeyDown(GLFW_KEY_S))
			camera->ProcessKeyboard(BACKWARD, deltaTime);
		if (IsKeyDown(GLFW_KEY_A))
			camera->ProcessKeyboard(LEFT, deltaTime);
		if (IsKeyDown(GLFW_KEY_D))
			camera->ProcessKeyboard(RIGHT, deltaTime);
	}

	void Renderer::Render()
	{
		initialGBufferStage->PerformStage(camera, model);
	}

	void Renderer::OnKeyPressed(const int key)
	{
		keyState[key] = true;
	}

	void Renderer::OnKeyReleased(const int key)
	{
		keyState[key] = false;
	}

	void Renderer::OnMouseButtonPressed(const int button)
	{
		mouseState[button] = true;
	}

	void Renderer::OnMouseButtonReleased(const int button)
	{
		mouseState[button] = false;
	}

	void Renderer::OnMouseMoved(const float deltaX, const float deltaY)
	{
		if (IsMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT))
			camera->ProcessMouseMovement(deltaX, deltaY);
	}

	void Renderer::OnMouseScrolled(const float scrollAmount)
	{
		camera->ProcessMouseScroll(scrollAmount);
	}

	bool Renderer::IsKeyDown(int key)
	{
		auto state = keyState.find(key);
		return state == keyState.end() ? false : state->second;
	}

	bool Renderer::IsMouseButtonDown(int button)
	{
		auto state = mouseState.find(button);
		return state == mouseState.end() ? false : state->second;
	}
}