#include "glm\gtc\matrix_transform.hpp"

#include "filesystem.hpp"

#include "VertexData.h"
#include "PostProcessingStage.h"

using namespace std;

namespace AdvancedRenderer
{
	PostProcessingStage::PostProcessingStage()
	{
		blurShader = unique_ptr<Shader>(new Shader(FileSystem::getPath("Shaders/Final.vert.glsl").c_str(), FileSystem::getPath("Shaders/Final.frag.glsl").c_str()));
		
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
	}

	PostProcessingStage::~PostProcessingStage()
	{
		glDeleteVertexArrays(1, &vao);
		static GLuint buffers[] = { quadPosition, quadUV, quadIndex };
		glDeleteBuffers(sizeof(buffers) / sizeof(GLuint), buffers);
	}

	void PostProcessingStage::PerformStage(const float exposure, const DisplayMode displayMode, const unique_ptr<InitialGBufferStage>& gbuffers, const unique_ptr<OcclusionStage>& occlusionBuffer, const unique_ptr<LightingStage>& lightingBuffer, const unique_ptr<CompositeStage>& compositeBuffer, const unique_ptr<Camera>& camera) const
	{
		glDisable(GL_DEPTH_TEST);
		glViewport(0, 0, ScreenWidth, ScreenHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		gbuffers->BindDiffuseColorBuffer(GL_TEXTURE0);
		gbuffers->BindSpecularColorBuffer(GL_TEXTURE1);
		gbuffers->BindPositionBuffer(GL_TEXTURE2);
		gbuffers->BindNormalBuffer(GL_TEXTURE3);
		occlusionBuffer->BindOcclusionBuffer(GL_TEXTURE4);
		lightingBuffer->BindLightingBuffer(GL_TEXTURE5);
		compositeBuffer->BindCompositeBuffer(GL_TEXTURE6);
		blurShader->Use();
		glUniformMatrix4fv(glGetUniformLocation(blurShader->Program, "View"), 1, GL_FALSE, value_ptr(camera->GetViewMatrix()));
		glUniformMatrix4fv(glGetUniformLocation(blurShader->Program, "PreviousView"), 1, GL_FALSE, value_ptr(camera->PreviousView));
		glUniformMatrix4fv(glGetUniformLocation(blurShader->Program, "Projection"), 1, GL_FALSE, value_ptr(camera->GetProjectionMatrix()));
		glUniform1i(glGetUniformLocation(blurShader->Program, "DiffuseColorBuffer"), 0);
		glUniform1i(glGetUniformLocation(blurShader->Program, "SpecularColorBuffer"), 1);
		glUniform1i(glGetUniformLocation(blurShader->Program, "PositionBuffer"), 2);
		glUniform1i(glGetUniformLocation(blurShader->Program, "NormalBuffer"), 3);
		glUniform1i(glGetUniformLocation(blurShader->Program, "OcclusionBuffer"), 4);
		glUniform1i(glGetUniformLocation(blurShader->Program, "LightingBuffer"), 5);
		glUniform1i(glGetUniformLocation(blurShader->Program, "CompositeBuffer"), 6);
		glUniform1f(glGetUniformLocation(blurShader->Program, "Exposure"), exposure);
		glUniform1i(glGetUniformLocation(blurShader->Program, "DisplayMode"), (int)displayMode);
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, sizeof(QuadIndices) / sizeof(unsigned short), GL_UNSIGNED_SHORT, NULL);
		glBindVertexArray(0);
	}
}