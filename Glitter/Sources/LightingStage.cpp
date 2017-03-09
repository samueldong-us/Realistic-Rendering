#include "glm\gtc\matrix_transform.hpp"

#include "filesystem.hpp"

#include "Constants.h"
#include "VertexData.h"
#include "LightingStage.h"

using namespace std;
using namespace glm;

namespace AdvancedRenderer
{
	LightingStage::LightingStage()
	{
		shadowShader = unique_ptr<Shader>(new Shader(FileSystem::getPath("Shaders/Shadow.vert.glsl").c_str(), FileSystem::getPath("Shaders/Shadow.frag.glsl").c_str()));
		lightingShader = unique_ptr<Shader>(new Shader(FileSystem::getPath("Shaders/CookTorrance.vert.glsl").c_str(), FileSystem::getPath("Shaders/CookTorrance.frag.glsl").c_str()));

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

		glGenTextures(1, &shadowMap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, shadowMap);
		for (int i = 0; i < 6; i++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, ShadowTextureWidth, ShadowTextureHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glGenFramebuffers(1, &shadowFramebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowFramebuffer);

		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		glGenFramebuffers(1, &lightingFramebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, lightingFramebuffer);

		glGenTextures(1, &lightingTexture);
		glBindTexture(GL_TEXTURE_2D, lightingTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, ScreenWidth, ScreenHeight, 0, GL_RGB, GL_FLOAT, NULL);
		DefaultTextureParameters(GL_TEXTURE_2D);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightingTexture, 0);

		glDrawBuffer(GL_COLOR_ATTACHMENT0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	LightingStage::~LightingStage()
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteFramebuffers(1, &shadowFramebuffer);
		glDeleteFramebuffers(1, &lightingFramebuffer);
		glDeleteTextures(1, &shadowMap);
		glDeleteTextures(1, &lightingTexture);
		static GLuint buffers[] = { quadPosition, quadUV, quadIndex };
		glDeleteBuffers(sizeof(buffers) / sizeof(GLuint), buffers);
	}

	void LightingStage::PerformStage(const unique_ptr<InitialGBufferStage>& gbuffers, const vector<unique_ptr<Light>*>& lights, const unique_ptr<Model>& model, const mat4& modelMatrix, const unique_ptr<Camera>& camera) const
	{
		static vec3 lightOffset[] =
		{
			vec3(1.0f, 0.0f, 0.0f),
			vec3(-1.0f, 0.0f, 0.0f),
			vec3(0.0f, 1.0f, 0.0f),
			vec3(0.0f, -1.0f, 0.0f),
			vec3(0.0f, 0.0f, 1.0f),
			vec3(0.0f, 0.0f, -1.0f)
		};
		static vec3 lightUp[] =
		{
			vec3(0.0f, -1.0f, 0.0f),
			vec3(0.0f, -1.0f, 0.0f),
			vec3(0.0f, 0.0f, 1.0f),
			vec3(0.0f, 0.0f, -1.0f),
			vec3(0.0f, -1.0f, 0.0f),
			vec3(0.0f, -1.0f, 0.0f)
		};
		static mat4 projection = perspective(radians(90.0f), 1.0f, mNear, mFar);
		glBindFramebuffer(GL_FRAMEBUFFER, lightingFramebuffer);
		glClear(GL_COLOR_BUFFER_BIT);
		for (auto light : lights)
		{
			glEnable(GL_DEPTH_TEST);
			glBindFramebuffer(GL_FRAMEBUFFER, shadowFramebuffer);
			glViewport(0, 0, ShadowTextureWidth, ShadowTextureHeight);
			shadowShader->Use();
			glUniformMatrix4fv(glGetUniformLocation(shadowShader->Program, "Projection"), 1, GL_FALSE, value_ptr(projection));
			glUniformMatrix4fv(glGetUniformLocation(shadowShader->Program, "Model"), 1, GL_FALSE, value_ptr(modelMatrix));
			glUniform3fv(glGetUniformLocation(shadowShader->Program, "LightPosition"), 1, value_ptr((*light)->position));
			glUniform1f(glGetUniformLocation(shadowShader->Program, "FarPlane"), mFar);
			for (int i = 0; i < 6; i++)
			{
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, shadowMap, 0);
				glClear(GL_DEPTH_BUFFER_BIT);
				mat4 view = lookAt((*light)->position, (*light)->position + lightOffset[i], lightUp[i]);
				glUniformMatrix4fv(glGetUniformLocation(shadowShader->Program, "View"), 1, GL_FALSE, value_ptr(view));
				
				model->Draw(*shadowShader);
			}

			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE);
			glBindFramebuffer(GL_FRAMEBUFFER, lightingFramebuffer);
			glViewport(0, 0, ScreenWidth, ScreenHeight);
			gbuffers->BindDiffuseColorBuffer(GL_TEXTURE0);
			gbuffers->BindSpecularColorBuffer(GL_TEXTURE1);
			gbuffers->BindPositionBuffer(GL_TEXTURE2);
			gbuffers->BindNormalBuffer(GL_TEXTURE3);
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_CUBE_MAP, shadowMap);
			lightingShader->Use();
			glUniformMatrix4fv(glGetUniformLocation(lightingShader->Program, "View"), 1, GL_FALSE, value_ptr(camera->GetViewMatrix()));
			glUniform3fv(glGetUniformLocation(lightingShader->Program, "LightPosition"), 1, value_ptr((*light)->position));
			glUniform3fv(glGetUniformLocation(lightingShader->Program, "LightColor"), 1, value_ptr((*light)->color));
			glUniform1f(glGetUniformLocation(lightingShader->Program, "LightIntensity"), (*light)->intensity);
			glUniform1i(glGetUniformLocation(lightingShader->Program, "DiffuseColorBuffer"), 0);
			glUniform1i(glGetUniformLocation(lightingShader->Program, "SpecularColorBuffer"), 1);
			glUniform1i(glGetUniformLocation(lightingShader->Program, "PositionBuffer"), 2);
			glUniform1i(glGetUniformLocation(lightingShader->Program, "NormalBuffer"), 3);
			glUniform1i(glGetUniformLocation(lightingShader->Program, "ShadowMap"), 4);
			glUniform1f(glGetUniformLocation(lightingShader->Program, "FarPlane"), mFar);
			glBindVertexArray(vao);
			glDrawElements(GL_TRIANGLES, sizeof(QuadIndices) / sizeof(unsigned short), GL_UNSIGNED_SHORT, NULL);
			glBindVertexArray(0);
			glDisable(GL_BLEND);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void LightingStage::BindLightingBuffer(const GLenum textureUnit) const
	{
		glActiveTexture(textureUnit);
		glBindTexture(GL_TEXTURE_2D, lightingTexture);
	}
}