#include <iostream>
#include <random>

#include <glm/gtc/matrix_transform.hpp>

#include "filesystem.hpp"

#include "Constants.h"
#include "VertexData.h"
#include "OcclusionStage.h"

using namespace std;
using namespace glm;

namespace AdvancedRenderer
{
	OcclusionStage::OcclusionStage()
	{
		shader = unique_ptr<Shader>(new Shader(FileSystem::getPath("Shaders/Occlusion.vert.glsl").c_str(), FileSystem::getPath("Shaders/Occlusion.frag.glsl").c_str()));
		test = unique_ptr<Shader>(new Shader(FileSystem::getPath("Shaders/Testing.vert.glsl").c_str(), FileSystem::getPath("Shaders/Testing.frag.glsl").c_str()));
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
		glBindVertexArray(0);

		default_random_engine engine;
		normal_distribution<float> normal;
		uniform_real_distribution<float> uniform;
		for (int i = 0; i < OcclusionSamples; i++)
		{
			float x1 = normal(engine);
			float x2 = normal(engine);
			float x3 = normal(engine);
			float u = uniform(engine);
			float normalizer = powf(u, 1.0f / 3.0f) / sqrtf(x1 * x1 + x2 * x2 + x3 * x3);
			randomKernel[3 * i + 0] = x1 * normalizer;
			randomKernel[3 * i + 1] = x2 * normalizer;
			randomKernel[3 * i + 2] = x3 * normalizer;
		}

		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		glGenTextures(1, &occlusionTexture);
		glBindTexture(GL_TEXTURE_2D, occlusionTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, OcclusionTextureWidth, OcclusionTextureHeight, 0, GL_RGB, GL_FLOAT, NULL);
		DefaultTextureParameters();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, occlusionTexture, 0);

		glDrawBuffer(GL_COLOR_ATTACHMENT0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			cout << "Error: Framebuffer is not complete!: " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << endl;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	OcclusionStage::~OcclusionStage()
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteFramebuffers(1, &framebuffer);
		glDeleteTextures(1, &occlusionTexture);
		static GLuint buffers[] = { quadPosition, quadUV, quadIndex };
		glDeleteBuffers(sizeof(buffers) / sizeof(GLuint), buffers);
	}

	void OcclusionStage::PerformStage(const bool directional, const float radius, const unique_ptr<InitialGBufferStage>& gbuffers, const unique_ptr<Camera>& camera, const unique_ptr<Skybox>& environmentMap) const
	{
		glDisable(GL_DEPTH_TEST);
		glViewport(0, 0, OcclusionTextureWidth, OcclusionTextureHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glClear(GL_COLOR_BUFFER_BIT);
		if (directional)
		{
			gbuffers->BindDiffuseColorBuffer(GL_TEXTURE0);
			gbuffers->BindPositionBuffer(GL_TEXTURE1);
			gbuffers->BindNormalBuffer(GL_TEXTURE2);
			gbuffers->BindDepthBuffer(GL_TEXTURE3);
			environmentMap->BindSkybox(GL_TEXTURE4);
			shader->Use();
			glUniform1f(glGetUniformLocation(shader->Program, "Radius"), radius);
			glUniform3fv(glGetUniformLocation(shader->Program, "Kernel"), OcclusionSamples, randomKernel);
			glUniformMatrix4fv(glGetUniformLocation(shader->Program, "View"), 1, GL_FALSE, value_ptr(camera->GetViewMatrix()));
			glUniformMatrix4fv(glGetUniformLocation(shader->Program, "Projection"), 1, GL_FALSE, value_ptr(camera->GetProjectionMatrx()));
			glUniform1i(glGetUniformLocation(shader->Program, "DiffuseColorBuffer"), 0);
			glUniform1i(glGetUniformLocation(shader->Program, "PositionBuffer"), 1);
			glUniform1i(glGetUniformLocation(shader->Program, "NormalBuffer"), 2);
			glUniform1i(glGetUniformLocation(shader->Program, "DepthBuffer"), 3);
			glUniform1i(glGetUniformLocation(shader->Program, "Environment"), 4);
			glBindVertexArray(vao);
			glDrawElements(GL_TRIANGLES, sizeof(QuadIndices) / sizeof(unsigned short), GL_UNSIGNED_SHORT, NULL);
			glBindVertexArray(0);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, ScreenWidth, ScreenHeight);
		glClear(GL_COLOR_BUFFER_BIT);
		test->Use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, occlusionTexture);
		glUniform1i(glGetUniformLocation(test->Program, "Buffer"), 0);
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, sizeof(QuadIndices) / sizeof(unsigned short), GL_UNSIGNED_SHORT, NULL);
		glBindVertexArray(0);
	}
}