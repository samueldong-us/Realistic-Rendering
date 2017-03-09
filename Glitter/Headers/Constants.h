#pragma once
#include "glitter.hpp"

namespace AdvancedRenderer
{
	const int ShadowTextureWidth = 1024;
	const int ShadowTextureHeight = 1024;

	const int ScreenWidth = mWidth;
	const int ScreenHeight = mHeight;

	const int OcclusionSamples = 32;
	const int OcclusionTextureWidth = ScreenWidth;
	const int OcclusionTextureHeight = ScreenHeight;

	const float Ambient = 1.0f;

	inline void DefaultTextureParameters(const GLenum texture)
	{
		glTexParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
}