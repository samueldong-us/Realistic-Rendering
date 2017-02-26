#include <string>

#include <soil/soil.h>

#include "shader.hpp"

#include "Skybox.hpp"

using namespace std;

namespace AdvancedRenderer
{
	Skybox::Skybox(const string& filenamePrefix, const string& extension)
	{
		static string suffixes[] = {"rt", "lf", "up", "dn", "bk", "ft"};

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
		for (int i = 0; i < 6; i++)
		{
			int width, height;
			unsigned char* image = SOIL_load_image((filenamePrefix + "_" + suffixes[i] + "." + extension).c_str(), &width, &height, NULL, SOIL_LOAD_RGB);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
			SOIL_free_image_data(image);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		static GLfloat skyboxVertices[] = {
			// Positions          
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f,  1.0f
		};

		glGenBuffers(1, &positions);
		glBindBuffer(GL_ARRAY_BUFFER, positions);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, positions);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glBindVertexArray(0);
	}

	Skybox::~Skybox()
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteTextures(1, &texture);
		glDeleteBuffers(1, &positions);
	}

	void Skybox::Draw(Shader shader)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
		glUniform1i(glGetUniformLocation(shader.Program, "skybox"), 0);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}