#pragma once
#include "glitter.hpp"

namespace AdvancedRenderer
{
	const int ShadowTextureWidth = 1024;
	const int ShadowTextureHeight = 1024;

	const int ScreenWidth = mWidth;
	const int ScreenHeight = mHeight;

	const int OcclusionSamples = 64;
	const int OcclusionTextureWidth = ScreenWidth;
	const int OcclusionTextureHeight = ScreenHeight;

	inline void DefaultTextureParameters()
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
}