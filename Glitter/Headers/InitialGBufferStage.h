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

		void PerformStage(const std::unique_ptr<Camera>& camera, const std::unique_ptr<Model>& model, const glm::mat4& modelMatrix) const;

		void BindDiffuseColorBuffer(const GLenum textureUnit) const;
		void BindSpecularColorBuffer(const GLenum textureUnit) const;
		void BindPositionBuffer(const GLenum textureUnit) const;
		void BindNormalBuffer(const GLenum textureUnit) const;
		void BindDepthBuffer(const GLenum textureUnit) const;
		void BindFramebuffer(const GLenum framebuffer) const;

	private:
		std::unique_ptr<Shader> shader;
		GLuint framebuffer, diffuseColorTexture, specularColorTexture, positionTexture, normalTexture, depthTexture;
	};
}