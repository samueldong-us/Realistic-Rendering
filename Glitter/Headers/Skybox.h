#pragma once
#include <memory>
#include <string>

#include "glad/glad.h"

#include "shader.hpp"

namespace AdvancedRenderer
{
	class Skybox
	{
	public:
		Skybox(const std::string& filenamePrefix, const std::string& extension);
		~Skybox();

		void BindSkybox(const GLenum textureUnit) const;

	private:
		GLuint texture;
	};
}