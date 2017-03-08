#pragma once
#include <memory>
#include <map>

#include "camera.hpp"
#include "model.hpp"

#include "Skybox.h"
#include "InitialGBufferStage.h"
#include "OcclusionStage.h"

namespace AdvancedRenderer
{
	class Renderer
	{
	public:
		void InitializeOpenGL();
		void LoadContent();
		void Update(const float deltaTime);
		void Render();
		void OnKeyPressed(const int key);
		void OnKeyReleased(const int key);
		void OnMouseButtonPressed(const int button);
		void OnMouseButtonReleased(const int button);
		void OnMouseMoved(const float deltaX, const float deltaY);
		void OnMouseScrolled(const float scrollAmount);

	private:
		enum class OcclusionMode { AmbientOcclusion, DirectionalOcclusion };
		OcclusionMode occlusionMode;

		std::unique_ptr<InitialGBufferStage> initialGBufferStage;
		std::unique_ptr<OcclusionStage> occlusionStage;

		std::unique_ptr<Camera> camera;
		std::unique_ptr<Model> model;
		std::unique_ptr<Skybox> environment;

		std::map<int, bool> keyState;
		std::map<int, bool> mouseState;

		bool IsKeyDown(int key);
		bool IsMouseButtonDown(int button);
	};

	/*
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_STENCIL_TEST);

	deferredShader = new Shader(FileSystem::getPath("Shaders/geometry.vert.glsl").c_str(), FileSystem::getPath("Shaders/geometry.frag.glsl").c_str());
	postShader = new Shader(FileSystem::getPath("Shaders/deferred.vert.glsl").c_str(), FileSystem::getPath("Shaders/deferred.frag.glsl").c_str());

	Shader skyboxShader(FileSystem::getPath("Shaders/skybox.vert.glsl").c_str(), FileSystem::getPath("Shaders/skybox.frag.glsl").c_str());
	Skybox skybox(FileSystem::getPath("Resources/clouds/bluecloud"), "jpg");

	Model sponzaModel(FileSystem::getPath("Resources/crytek_sponza/sponza.obj").c_str());

	Light test(0.0f, 10.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f);

	Initialize();
	*/

	/*
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilMask(0xFF);

	// Background Fill Color
	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
	glBindFramebuffer(GL_FRAMEBUFFER, deferredBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glStencilMask(0x00);
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (GLfloat)mWidth / (GLfloat)mHeight, mNear, mFar);
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 model = glm::mat4();
	deferredShader->Use();
	model = glm::translate(model, glm::vec3(-1, 0, 0));
	glUniformMatrix4fv(glGetUniformLocation(deferredShader->Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(deferredShader->Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(deferredShader->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	//RenderCube();

	model = glm::mat4();
	model = glm::scale(model, glm::vec3(0.05f));    // The sponza model is too big, scale it first
	glUniformMatrix4fv(glGetUniformLocation(deferredShader->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	sponzaModel.Draw(*deferredShader);

	glStencilMask(0xFF);
	skyboxShader.Use();
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
	skybox.Draw(skyboxShader);
	glStencilMask(0x00);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	postShader->Use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseColor);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specularColor);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, position);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, normal);
	glUniform1i(glGetUniformLocation(postShader->Program, "diffuseBuffer"), 0);
	glUniform1i(glGetUniformLocation(postShader->Program, "specularBuffer"), 1);
	glUniform1i(glGetUniformLocation(postShader->Program, "positionBuffer"), 2);
	glUniform1i(glGetUniformLocation(postShader->Program, "normalBuffer"), 3);
	glUniform1i(glGetUniformLocation(postShader->Program, "debug"), debug);
	glUniform1f(glGetUniformLocation(postShader->Program, "ambientFactor"), 0.5f);
	glUniform1f(glGetUniformLocation(postShader->Program, "specularExponent"), 32.0f);
	glUniformMatrix4fv(glGetUniformLocation(postShader->Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
	test.AttachTo(*postShader, "lights[0]");
	glBindVertexArray(quadVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);
	glBindVertexArray(0);
	*/

	/*
	void Initialize()
	{
	float quadPositions[] = {
	-1.0f, -1.0f, 0.0f, 1.0f,
	1.0f, -1.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 0.0f, 1.0f,
	-1.0f, 1.0f, 0.0f, 1.0f
	};

	float quadUVs[] = {
	0.0f, 0.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f
	};

	unsigned short quadIndices[] = { 0, 1, 2, 2, 3, 0};

	GLuint quadPosition, quadUV, quadIndex;

	glGenBuffers(1, &quadPosition);
	glBindBuffer(GL_ARRAY_BUFFER, quadPosition);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadPositions), quadPositions, GL_STATIC_DRAW);

	glGenBuffers(1, &quadUV);
	glBindBuffer(GL_ARRAY_BUFFER, quadUV);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadUVs), quadUVs, GL_STATIC_DRAW);

	glGenBuffers(1, &quadIndex);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIndex);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &quadVAO);
	glBindVertexArray(quadVAO);

	glEnableVertexAttribArray(glGetAttribLocation(postShader->Program, "position"));
	glBindBuffer(GL_ARRAY_BUFFER, quadPosition);
	glVertexAttribPointer(glGetAttribLocation(postShader->Program, "position"), 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(glGetAttribLocation(postShader->Program, "texCoords"));
	glBindBuffer(GL_ARRAY_BUFFER, quadUV);
	glVertexAttribPointer(glGetAttribLocation(postShader->Program, "texCoords"), 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIndex);

	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
	glGenFramebuffers(1, &deferredBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, deferredBuffer);

	glGenTextures(1, &diffuseColor);
	glBindTexture(GL_TEXTURE_2D, diffuseColor);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, diffuseColor, 0);

	glGenTextures(1, &specularColor);
	glBindTexture(GL_TEXTURE_2D, specularColor);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, specularColor, 0);

	glGenTextures(1, &position);
	glBindTexture(GL_TEXTURE_2D, position);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, mWidth, mHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, position, 0);

	glGenTextures(1, &normal);
	glBindTexture(GL_TEXTURE_2D, normal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, mWidth, mHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, normal, 0);

	GLuint attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, attachments);

	GLuint depthBuffer;
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mWidth, mHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
	cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	*/
}