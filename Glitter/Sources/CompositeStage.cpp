#include <iostream>

#include "glm\gtc\matrix_transform.hpp"

#include "filesystem.hpp"

#include "Constants.h"
#include "VertexData.h"
#include "CompositeStage.h"

using namespace std;
using namespace glm;

namespace AdvancedRenderer
{
	CompositeStage::CompositeStage()
	{
		compositeShader = unique_ptr<Shader>(new Shader(FileSystem::getPath("Shaders/Composite.vert.glsl").c_str(), FileSystem::getPath("Shaders/Composite.frag.glsl").c_str()));
		lightShader = unique_ptr<Shader>(new Shader(FileSystem::getPath("Shaders/Light.vert.glsl").c_str(), FileSystem::getPath("Shaders/Light.frag.glsl").c_str()));
		skymapShader = unique_ptr<Shader>(new Shader(FileSystem::getPath("Shaders/Skymap.vert.glsl").c_str(), FileSystem::getPath("Shaders/Skymap.frag.glsl").c_str()));

		glGenBuffers(1, &quadPosition);
		glBindBuffer(GL_ARRAY_BUFFER, quadPosition);
		glBufferData(GL_ARRAY_BUFFER, sizeof(QuadPositions), QuadPositions, GL_STATIC_DRAW);

		glGenBuffers(1, &quadUV);
		glBindBuffer(GL_ARRAY_BUFFER, quadUV);
		glBufferData(GL_ARRAY_BUFFER, sizeof(QuadUVs), QuadUVs, GL_STATIC_DRAW);

		glGenBuffers(1, &quadIndex);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIndex);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(QuadIndices), QuadIndices, GL_STATIC_DRAW);

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, quadPosition);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, quadUV);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIndex);

		glGenBuffers(1, &cubePosition);
		glBindBuffer(GL_ARRAY_BUFFER, cubePosition);
		glBufferData(GL_ARRAY_BUFFER, sizeof(CubePositions), CubePositions, GL_STATIC_DRAW);

		glGenVertexArrays(1, &cubeVao);
		glBindVertexArray(cubeVao);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, cubePosition);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glBindVertexArray(0);

		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		glGenTextures(1, &compositeTexture);
		glBindTexture(GL_TEXTURE_2D, compositeTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, ScreenWidth, ScreenHeight, 0, GL_RGB, GL_FLOAT, NULL);
		DefaultTextureParameters(GL_TEXTURE_2D);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, compositeTexture, 0);

		glDrawBuffer(GL_COLOR_ATTACHMENT0);

		GLuint depthBuffer;
		glGenRenderbuffers(1, &depthBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, ScreenWidth, ScreenHeight);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			cout << "Error: Framebuffer is not complete!: " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << endl;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	CompositeStage::~CompositeStage()
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteFramebuffers(1, &framebuffer);
		glDeleteTextures(1, &compositeTexture);
		static GLuint buffers[] = { quadPosition, quadUV, quadIndex };
		glDeleteBuffers(sizeof(buffers) / sizeof(GLuint), buffers);
	}

	void CompositeStage::PerformStage(const std::unique_ptr<InitialGBufferStage>& gbuffers, const std::unique_ptr<OcclusionStage>& occlusionBuffer, const std::unique_ptr<LightingStage>& lightingBuffer, const vector<unique_ptr<Light>*>& lights, const int activeLight, const std::unique_ptr<Skybox>& skybox, const std::unique_ptr<Camera>& camera) const
	{
		gbuffers->BindFramebuffer(GL_READ_FRAMEBUFFER);
		glViewport(0, 0, ScreenWidth, ScreenHeight);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
		glBlitFramebuffer(0, 0, ScreenWidth, ScreenHeight, 0, 0, ScreenWidth, ScreenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		occlusionBuffer->BindOcclusionBuffer(GL_TEXTURE0);
		lightingBuffer->BindLightingBuffer(GL_TEXTURE1);
		gbuffers->BindDiffuseColorBuffer(GL_TEXTURE2);
		compositeShader->Use();
		glUniform1i(glGetUniformLocation(compositeShader->Program, "OcclusionBuffer"), 0);
		glUniform1i(glGetUniformLocation(compositeShader->Program, "LightingBuffer"), 1);
		glUniform1i(glGetUniformLocation(compositeShader->Program, "DiffuseColorBuffer"), 2);
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, sizeof(QuadIndices) / sizeof(unsigned short), GL_UNSIGNED_SHORT, NULL);
		glEnable(GL_DEPTH_TEST);
		lightShader->Use();
		glUniformMatrix4fv(glGetUniformLocation(lightShader->Program, "Projection"), 1, GL_FALSE, value_ptr(camera->GetProjectionMatrix()));
		glUniformMatrix4fv(glGetUniformLocation(lightShader->Program, "View"), 1, GL_FALSE, value_ptr(camera->GetViewMatrix()));
		glBindVertexArray(cubeVao);
		for (int i = 0; i < lights.size(); i++)
		{
			unique_ptr<Light>* light = lights[i];
			glUniformMatrix4fv(glGetUniformLocation(lightShader->Program, "Model"), 1, GL_FALSE, value_ptr(scale(translate(mat4(), (*light)->position), vec3(0.5f))));
			glUniform3fv(glGetUniformLocation(lightShader->Program, "Color"), 1, value_ptr((*light)->color));
			glUniform1f(glGetUniformLocation(lightShader->Program, "Multiplier"), i == activeLight ? 2.0f : 0.5f);
			glDrawArrays(GL_TRIANGLES, 0, sizeof(CubePositions) / sizeof(float));
		}

		skybox->BindSkybox(GL_TEXTURE0);
		skymapShader->Use();
		glUniformMatrix4fv(glGetUniformLocation(skymapShader->Program, "Projection"), 1, GL_FALSE, value_ptr(camera->GetProjectionMatrix()));
		glUniformMatrix4fv(glGetUniformLocation(skymapShader->Program, "View"), 1, GL_FALSE, value_ptr(camera->GetViewMatrix()));
		glUniform1i(glGetUniformLocation(skymapShader->Program, "Skymap"), 0);
		glBindVertexArray(cubeVao);
		glDrawArrays(GL_TRIANGLES, 0, sizeof(CubePositions) / sizeof(float));
		glBindVertexArray(0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
	}

	void CompositeStage::BindCompositeBuffer(const GLenum textureUnit) const
	{
		glActiveTexture(textureUnit);
		glBindTexture(GL_TEXTURE_2D, compositeTexture);
	}
}