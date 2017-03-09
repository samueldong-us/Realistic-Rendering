#include "Light.h"
namespace AdvancedRenderer
{
	Light::Light(const glm::vec3 & position, const glm::vec3 & color, const float intensity)
	{
		this->position = position;
		this->color = color;
		this->intensity = intensity;
	}
}