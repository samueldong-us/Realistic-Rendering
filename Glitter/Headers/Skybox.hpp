#pragma once

#include <string>

#include <glad/glad.h>

#include "shader.hpp"

namespace AdvancedRenderer
{
	class Skybox
	{
	public:
		Skybox(const std::string& filenamePrefix, const std::string& extension);
		~Skybox();
		
		void Draw(Shader shader);

	private:
		GLuint vao, texture, positions;
	};
}