#pragma once
#include <memory>

#include "camera.hpp"

#include "Constants.h"
#include "InitialGBufferStage.h"
#include "OcclusionStage.h"
#include "LightingStage.h"
#include "CompositeStage.h"

namespace AdvancedRenderer
{
	class PostProcessingStage
	{
	public:
		PostProcessingStage();
		~PostProcessingStage();

		void PerformStage(const float exposure, const DisplayMode displayMode, const std::unique_ptr<InitialGBufferStage>& gbuffers, const std::unique_ptr<OcclusionStage>& occlusionBuffer, const std::unique_ptr<LightingStage>& lightingBuffer, const std::unique_ptr<CompositeStage>& compositeBuffer, const std::unique_ptr<Camera>& camera) const;

	private:
		std::unique_ptr<Shader> blurShader;
		GLuint vao, quadPosition, quadUV, quadIndex;
	};
}