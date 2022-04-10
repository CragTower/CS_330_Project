#include "Texture.h"


Texture::Texture(const char* filename, GLenum location)
{	
	glGenTextures(1, &ID);
	//glActiveTexture(location);
	glBindTexture(GL_TEXTURE_2D, ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int width, height, nChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* image = stbi_load(filename, &width, &height, &nChannels, 0);
	
	if (image)
	{
		
		
		if (nChannels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Texture loading failed." << std::endl;
	}

	stbi_image_free(image);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::texLoc(Shader& shader, const char* uniform, GLuint unit)
{
	GLuint textLoc = glGetUniformLocation(shader.ID, uniform);
	shader.Activate();
	glUniform1i(textLoc, unit);
}
void Texture::Bind()
{
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::Unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Delete()
{
	glDeleteTextures(1, &ID);
}