#pragma once
#include <memory>

#include "glad\glad.h"

#include "shader.hpp"
#include "camera.hpp"
#include "model.hpp"

namespace AdvancedRenderer
{
	class InitialGBufferStage
	{
	public:
		InitialGBufferStage();
		~InitialGBufferStage();

		void PerformStage(const std::unique_ptr<Camera>& camera, const std::unique_ptr<Model>& model);

	private:
		std::unique_ptr<Shader> shader;
		GLuint framebuffer, diffuseColorTexture, specularColorTexture, positionTexture, normalTexture;

		inline void DefaultTextureParameters() const;
	};
}