#include "soil/soil.h"

extern "C"
{
#include "rgbe.h"
}

#include "VertexData.h"
#include "Skybox.h"

using namespace std;

namespace AdvancedRenderer
{
	Skybox::Skybox(const string& filenamePrefix, const string& extension)
	{
		static string suffixes[] = {"rt", "lf", "up", "dn", "bk", "ft"};

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
		if (extension.compare("hdr") == 0)
		{
			for (int i = 0; i < 6; i++)
			{
				FILE* imageFile = fopen((filenamePrefix + "_" + suffixes[i] + "." + extension).c_str(), "rb");
				int width, height;
				RGBE_ReadHeader(imageFile, &width, &height, NULL);
				float* image = (float*)malloc(sizeof(float) * 3 * width * height);
				RGBE_ReadPixels_RLE(imageFile, image, width, height);
				fclose(imageFile);
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, image);
				free(image);
			}
		}
		else
		{
			for (int i = 0; i < 6; i++)
			{
				int width, height;
				unsigned char* image = SOIL_load_image((filenamePrefix + "_" + suffixes[i] + "." + extension).c_str(), &width, &height, NULL, SOIL_LOAD_RGB);
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
				SOIL_free_image_data(image);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	Skybox::~Skybox()
	{
		glDeleteTextures(1, &texture);
	}

	void Skybox::BindSkybox(const GLenum textureUnit) const
	{
		glActiveTexture(textureUnit);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	}
}