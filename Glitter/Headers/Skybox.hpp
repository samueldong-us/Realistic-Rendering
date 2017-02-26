#pragma once

#include <string>

#include <glad/glad.h>

namespace AdvancedRenderer
{
	class Skybox
	{
	public:
		Skybox(const std::string& filenamePrefix);
		~Skybox();
		
		void Draw();

	private:
		GLuint vao, texture;
	};
}