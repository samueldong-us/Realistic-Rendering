#include <iostream>

#include "glm/gtc/matrix_transform.hpp"

#include "filesystem.hpp"

#include "Constants.h"
#include "InitialGBufferStage.h"

using namespace std;
using namespace glm;

namespace AdvancedRenderer
{
	InitialGBufferStage::InitialGBufferStage()
	{
		shader = unique_ptr<Shader>(new Shader(FileSystem::getPath("Shaders/InitialGBufferStage.vert.glsl").c_str(), FileSystem::getPath("Shaders/InitialGBufferStage.frag.glsl").c_str()));

		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		glGenTextures(1, &diffuseColorTexture);
		glBindTexture(GL_TEXTURE_2D, diffuseColorTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, ScreenWidth, ScreenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		DefaultTextureParameters(GL_TEXTURE_2D);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, diffuseColorTexture, 0);

		glGenTextures(1, &specularColorTexture);
		glBindTexture(GL_TEXTURE_2D, specularColorTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, ScreenWidth, ScreenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		DefaultTextureParameters(GL_TEXTURE_2D);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, specularColorTexture, 0);

		glGenTextures(1, &positionTexture);
		glBindTexture(GL_TEXTURE_2D, positionTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, ScreenWidth, ScreenHeight, 0, GL_RGB, GL_FLOAT, NULL);
		DefaultTextureParameters(GL_TEXTURE_2D);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, positionTexture, 0);

		glGenTextures(1, &normalTexture);
		glBindTexture(GL_TEXTURE_2D, normalTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, ScreenWidth, ScreenHeight, 0, GL_RGB, GL_FLOAT, NULL);
		DefaultTextureParameters(GL_TEXTURE_2D);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, normalTexture, 0);

		glGenTextures(1, &depthTexture);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, ScreenWidth, ScreenHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		DefaultTextureParameters(GL_TEXTURE_2D);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

		static GLuint attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		glDrawBuffers(4, attachments);
		
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			cout << "Error: Framebuffer is not complete!: " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << endl;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	InitialGBufferStage::~InitialGBufferStage()
	{
		glDeleteFramebuffers(1, &framebuffer);
		static GLuint textures[] = { diffuseColorTexture, specularColorTexture, positionTexture, normalTexture, depthTexture };
		glDeleteTextures(sizeof(textures) / sizeof(GLuint), textures);
	}

	void InitialGBufferStage::PerformStage(const unique_ptr<Camera>& camera, const unique_ptr<Model>& model, const mat4& modelMatrix) const
	{
		glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, ScreenWidth, ScreenHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader->Use();
		glUniformMatrix4fv(glGetUniformLocation(shader->Program, "Projection"), 1, GL_FALSE, value_ptr(camera->GetProjectionMatrx()));
		glUniformMatrix4fv(glGetUniformLocation(shader->Program, "View"), 1, GL_FALSE, value_ptr(camera->GetViewMatrix()));
		glUniformMatrix4fv(glGetUniformLocation(shader->Program, "Model"), 1, GL_FALSE, value_ptr(modelMatrix));
		model->Draw(*shader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
	}

	void InitialGBufferStage::BindDiffuseColorBuffer(const GLenum textureUnit) const
	{
		glActiveTexture(textureUnit);
		glBindTexture(GL_TEXTURE_2D, diffuseColorTexture);
	}

	void InitialGBufferStage::BindSpecularColorBuffer(const GLenum textureUnit) const
	{
		glActiveTexture(textureUnit);
		glBindTexture(GL_TEXTURE_2D, specularColorTexture);
	}

	void InitialGBufferStage::BindPositionBuffer(const GLenum textureUnit) const
	{
		glActiveTexture(textureUnit);
		glBindTexture(GL_TEXTURE_2D, positionTexture);
	}

	void InitialGBufferStage::BindNormalBuffer(const GLenum textureUnit) const
	{
		glActiveTexture(textureUnit);
		glBindTexture(GL_TEXTURE_2D, normalTexture);
	}

	void InitialGBufferStage::BindDepthBuffer(const GLenum textureUnit) const
	{
		glActiveTexture(textureUnit);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
	}

	void InitialGBufferStage::BindFramebuffer(const GLenum framebuffer) const
	{
		glBindFramebuffer(framebuffer, this->framebuffer);
	}
}