#include <memory>
#include <iostream>

#include "glad\glad.h"
#include "glm\glm.hpp"

#include "glitter.hpp"
#include "filesystem.hpp"
#include "shader.hpp"

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
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, mWidth, mHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		DefaultTextureParameters();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, diffuseColorTexture, 0);

		glGenTextures(1, &specularColorTexture);
		glBindTexture(GL_TEXTURE_2D, specularColorTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, mWidth, mHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		DefaultTextureParameters();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, specularColorTexture, 0);

		glGenTextures(1, &positionTexture);
		glBindTexture(GL_TEXTURE_2D, positionTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, mWidth, mHeight, 0, GL_RGB, GL_FLOAT, NULL);
		DefaultTextureParameters();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, positionTexture, 0);

		glGenTextures(1, &normalTexture);
		glBindTexture(GL_TEXTURE_2D, normalTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, mWidth, mHeight, 0, GL_RGB, GL_FLOAT, NULL);
		DefaultTextureParameters();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, normalTexture, 0);

		GLuint depthStencilBuffer;
		glGenRenderbuffers(1, &depthStencilBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthStencilBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mWidth, mHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthStencilBuffer);

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
		static GLuint textures[] = { diffuseColorTexture, specularColorTexture, positionTexture, normalTexture};
		glDeleteTextures(5, textures);
	}

	void InitialGBufferStage::PerformStage(const unique_ptr<Camera>& camera, const unique_ptr<Model>& model)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		shader->Use();
		glUniformMatrix4fv(glGetUniformLocation(shader->Program, "Projection"), 1, GL_FALSE, value_ptr(camera->GetProjectionMatrx()));
		glUniformMatrix4fv(glGetUniformLocation(shader->Program, "View"), 1, GL_FALSE, value_ptr(camera->GetViewMatrix()));
		glUniformMatrix4fv(glGetUniformLocation(shader->Program, "Model"), 1, GL_FALSE, value_ptr(scale(mat4(), vec3(0.05f))));
		model->Draw(*shader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void InitialGBufferStage::DefaultTextureParameters() const
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
}