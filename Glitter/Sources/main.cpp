// Local Headers
#include "glitter.hpp"
// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Standard Headers
#include <cstdio>
#include <cstdlib>

//// Helper functions
#include <camera.hpp>
#include <shader.hpp>
#include <model.hpp>
#include <filesystem.hpp>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

Shader* deferredShader;
Shader* postShader;

GLuint deferredBuffer;
GLuint diffuseColor, specularColor, position, normal;

GLuint quadVAO;
void Initialize();

Camera camera(glm::vec3(0.f, 0.f, 2.f));

int main(int argc, char * argv[]) {

	// Load GLFW and Create a Window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	auto mWindow = glfwCreateWindow(mWidth, mHeight, "OpenGL", nullptr, nullptr);

	// Check for Valid Context
	if (mWindow == nullptr) {
		fprintf(stderr, "Failed to Create OpenGL Context");
		return EXIT_FAILURE;
	}

	// Create Context and Load OpenGL Functions
	glfwMakeContextCurrent(mWindow);
	gladLoadGL();
	fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

	// Set callback functions
	glfwSetKeyCallback(mWindow, key_callback);
	glfwSetCursorPosCallback(mWindow, mouse_callback);
	glfwSetScrollCallback(mWindow, scroll_callback);
	//glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glEnable(GL_DEPTH_TEST);

	deferredShader = new Shader(FileSystem::getPath("Shaders/geometry.vert.glsl").c_str(), FileSystem::getPath("Shaders/geometry.frag.glsl").c_str());
	postShader = new Shader(FileSystem::getPath("Shaders/deferred.vert.glsl").c_str(), FileSystem::getPath("Shaders/deferred.frag.glsl").c_str());

	Model sponzaModel(FileSystem::getPath("Resources/crytek_sponza/sponza.obj").c_str());

	Initialize();

	// Rendering Loop
	while (glfwWindowShouldClose(mWindow) == false) {
		glfwPollEvents();
		
		// Background Fill Color
		glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
		glBindFramebuffer(GL_FRAMEBUFFER, deferredBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
		glUniform1i(glGetUniformLocation(postShader->Program, "diffuseColor"), 0);
		glUniform1i(glGetUniformLocation(postShader->Program, "specularColor"), 1);
		glUniform1i(glGetUniformLocation(postShader->Program, "position"), 2);
		glUniform1i(glGetUniformLocation(postShader->Program, "normal"), 3);
		glBindVertexArray(quadVAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);
		glBindVertexArray(0);

		// Flip Buffers and Draw
		glfwSwapBuffers(mWindow);
	}   glfwTerminate();
	return EXIT_SUCCESS;
}

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

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	// Camera movements
	if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT))
		camera.ProcessKeyboard(FORWARD, 0.1);
	if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT))
		camera.ProcessKeyboard(BACKWARD, 0.1);
	if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT))
		camera.ProcessKeyboard(LEFT, 0.1);
	if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT))
		camera.ProcessKeyboard(RIGHT, 0.1);
}

GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if (state == GLFW_PRESS) {
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		GLfloat xoffset = xpos - lastX;
		GLfloat yoffset = lastY - ypos;

		lastX = xpos;
		lastY = ypos;

		camera.ProcessMouseMovement(xoffset, yoffset);
	}
	if (state == GLFW_RELEASE) {
		firstMouse = true;
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}