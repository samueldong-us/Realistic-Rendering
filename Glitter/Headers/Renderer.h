#pragma once
#include <memory>
#include <map>

#include "camera.hpp"
#include "model.hpp"

#include "Skybox.h"
#include "Light.h"
#include "InitialGBufferStage.h"
#include "OcclusionStage.h"
#include "LightingStage.h"
#include "CompositeStage.h"
#include "PostProcessingStage.h"

namespace AdvancedRenderer
{
	class Renderer
	{
	public:
		void InitializeOpenGL();
		void LoadContent();
		void Update(const float deltaTime);
		void Render();
		void OnKeyPressed(const int key);
		void OnKeyReleased(const int key);
		void OnMouseButtonPressed(const int button);
		void OnMouseButtonReleased(const int button);
		void OnMouseMoved(const float deltaX, const float deltaY);
		void OnMouseScrolled(const float scrollAmount);

	private:
		DisplayMode displayMode;
		OcclusionMode occlusionMode;
		int activeLight;
		float exposure;

		std::unique_ptr<InitialGBufferStage> initialGBufferStage;
		std::unique_ptr<OcclusionStage> occlusionStage;
		std::unique_ptr<LightingStage> lightingStage;
		std::unique_ptr<CompositeStage> compositeStage;
		std::unique_ptr<PostProcessingStage> postProcessingStage;

		std::unique_ptr<Camera> camera;
		std::unique_ptr<Model> model;
		std::unique_ptr<Skybox> environment;
		std::unique_ptr<Light> light[3];
		std::vector<std::unique_ptr<Light>*> lights;

		std::map<int, bool> keyState;
		std::map<int, bool> mouseState;

		bool IsKeyDown(int key);
		bool IsMouseButtonDown(int button);
	};
}