#pragma once
#include <memory>

#include "glad\glad.h"

#include "shader.hpp"
#include "camera.hpp"

#include "Constants.h"
#include "Skybox.h"
#include "InitialGBufferStage.h"

namespace AdvancedRenderer
{
	class OcclusionStage
	{
	public:
		OcclusionStage();
		~OcclusionStage();

		void PerformStage(const bool directional, const float radius, const std::unique_ptr<InitialGBufferStage>& gbuffers, const std::unique_ptr<Camera>& camera, const std::unique_ptr<Skybox>& environmentMap) const;

	private:
		std::unique_ptr<Shader> shader, test;
		GLuint vao, framebuffer, occlusionTexture, quadPosition, quadUV, quadIndex;

		float randomKernel[OcclusionSamples * 3];
	};
}