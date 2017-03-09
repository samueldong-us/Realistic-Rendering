#pragma once

#include "glm\glm.hpp"

namespace AdvancedRenderer
{
	class Light
	{
	public:
		Light(const glm::vec3& position, const glm::vec3& color, const float intensity);

		glm::vec3 position, color;
		float intensity;
	};
}