#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

class Texture
{
public: 
	void flipImageVertically(unsigned char* image, int width, int height, int channels);
	bool UCreateTexture(const char* filename, GLuint& textureId);
	void UDestroyTexture(GLuint textureId);
};

