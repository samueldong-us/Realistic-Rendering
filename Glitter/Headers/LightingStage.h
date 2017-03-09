#pragma once
#include <memory>
#include <vector>

#include "glad\glad.h"

#include "shader.hpp"
#include "camera.hpp"
#include "model.hpp"

#include "Light.h"
#include "OcclusionStage.h"

namespace AdvancedRenderer
{
	class LightingStage
	{
	public:
		LightingStage();
		~LightingStage();

		void PerformStage(const std::unique_ptr<InitialGBufferStage>& gbuffers, const std::vector<std::unique_ptr<Light>*>& lights, const std::unique_ptr<Model>& model, const glm::mat4& modelMatrix, const std::unique_ptr<Camera>& camera) const;
		void BindLightingBuffer(const GLenum textureUnit) const;

	private:
		std::unique_ptr<Shader> shadowShader, lightingShader;
		GLuint vao, shadowFramebuffer, shadowMap, lightingFramebuffer, lightingTexture, quadPosition, quadUV, quadIndex;
	};
}