#pragma once
#include <memory>

#include "glad\glad.h"

#include "shader.hpp"
#include "camera.hpp"

#include "Skybox.h"
#include "InitialGBufferStage.h"
#include "OcclusionStage.h"
#include "LightingStage.h"

namespace AdvancedRenderer
{
	class CompositeStage
	{
	public:
		CompositeStage();
		~CompositeStage();

		void PerformStage(const std::unique_ptr<InitialGBufferStage>& gbuffers, const std::unique_ptr<OcclusionStage>& occlusionBuffer, const std::unique_ptr<LightingStage>& lightingBuffer, const std::vector<std::unique_ptr<Light>*>& lights, const int activeLight, const std::unique_ptr<Skybox>& skybox, const std::unique_ptr<Camera>& camera) const;
		void BindCompositeBuffer(const GLenum textureUnit) const;

	private:
		std::unique_ptr<Shader> compositeShader, lightShader, skymapShader;
		GLuint vao, framebuffer, compositeTexture, quadPosition, quadUV, quadIndex, cubeVao, cubePosition;
	};
}