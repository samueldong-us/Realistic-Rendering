#include "GLFW\glfw3.h"

#include "filesystem.hpp"

#include "Renderer.h"

using namespace std;
using namespace glm;

namespace AdvancedRenderer
{
	void Renderer::InitializeOpenGL()
	{
		glDepthFunc(GL_LEQUAL);
	}

	void Renderer::LoadContent()
	{
		initialGBufferStage = unique_ptr<InitialGBufferStage>(new InitialGBufferStage());
		occlusionStage = unique_ptr<OcclusionStage>(new OcclusionStage());
		lightingStage = unique_ptr<LightingStage>(new LightingStage());
		compositeStage = unique_ptr<CompositeStage>(new CompositeStage());

		camera = unique_ptr<Camera>(new Camera(vec3(0.0f, 0.0f, 2.0f)));
		model = unique_ptr<Model>(new Model(FileSystem::getPath("Resources/crytek_sponza/sponza.obj")));
		environment = unique_ptr<Skybox>(new Skybox(FileSystem::getPath("Resources/grace_cathedral/grace"), "hdr"));

		light[0] = unique_ptr<Light>(new Light(vec3(0.0f, 20.0f, 0.0f), vec3(1.0f, 0.5f, 0.5f), 1000.0f));
		light[1] = unique_ptr<Light>(new Light(vec3(20.0f, 20.0f, 0.0f), vec3(0.5f, 1.0f, 0.5f), 1000.0f));
		light[2] = unique_ptr<Light>(new Light(vec3(-20.0f, 20.0f, 0.0f), vec3(0.5f, 0.5f, 1.0f), 1000.0f));
		lights.push_back(&light[0]);
		lights.push_back(&light[1]);
		lights.push_back(&light[2]);
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
		initialGBufferStage->PerformStage(camera, model, scale(mat4(), vec3(0.05f)));
		occlusionStage->PerformStage(true, 0.5f, initialGBufferStage, camera, environment);
		lightingStage->PerformStage(initialGBufferStage, lights, model, scale(mat4(), vec3(0.05f)), camera);
		compositeStage->PerformStage(initialGBufferStage, occlusionStage, lightingStage, lights, environment, camera);
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