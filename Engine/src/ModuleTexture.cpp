#include "ModuleTexture.h"
#include "ModuleRenderExercise.h"
#include "Libraries/DevIL-SDK/include/IL/ilu.h"
#include "DebugLeaks.h"

bool ModuleTexture::Init()
{
	LOG("Creating ModuleTexture context");

	ilInit();				//Initialization of DevIL

	return true;
}

GLuint ModuleTexture::Load(const char* path)
{
	ILuint imageId;
	GLuint texture = -1;
	ilGenImages(1, &imageId); //Generation of one image name
	ilBindImage(imageId);		//Binding of image name

	ILboolean ok = ilLoadImage(path);
	if (ok)
	{
		ok = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);	//Convert every colour component into unsigned byte. If your image contains alpha channel you can replace IL_RGB with IL_RGBA
		if (!ok)
		{
			LOG("ERROR ilConvertImage");
		}
		glGenTextures(1, &texture);				//Texture name generation
		glBindTexture(GL_TEXTURE_2D, texture);	//Binding of texture name
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//We will use linear interpolation for magnification filter
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//We will use linear interpolation for minifying filter

		ILinfo info;
		iluGetImageInfo(&info);
		if(info.Origin == IL_ORIGIN_UPPER_LEFT)
			iluFlipImage(); //this must be called before glTexImage2D

		glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH),
			ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE,
			ilGetData()); //Texture specification
	}
	else
	{
		LOG("ERROR ilLoadImage");
	}
	ilDeleteImages(1, &imageId); //Because we have already copied image data into texture data we can release memory used by image.

	return texture;
}

void ModuleTexture::DeleteTexture(unsigned int textureId)
{
	glDeleteTextures(1, &textureId);
}

