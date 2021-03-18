
#include "Texture2D.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture2D::Texture2D(const std::string & filename, int unit)
	:
	m_ID(0),
	m_unit(0)
{
	open(filename);
}

void Texture2D::load(const std::string & filename, int unit)
{
	m_unit = unit;

	open(filename);
}

void Texture2D::release()
{
	glDeleteTextures(1, &m_ID);
	m_ID = -1;
}

void Texture2D::setUnit(int unit)
{
	m_unit = unit;
	glBindTexture(GL_TEXTURE_2D, m_ID);
	glBindTextureUnit(GL_TEXTURE0 + m_unit, m_ID);
	glBindTexture(GL_TEXTURE_2D, 0);
}

int Texture2D::getUnit() const
{
	return m_unit;
}

int Texture2D::getWidth() const
{
	return m_width;
}

int Texture2D::getHeight() const
{
	return m_height;
}

unsigned int Texture2D::getID() const
{
	return m_ID;
}

void Texture2D::bind()
{
	glActiveTexture(GL_TEXTURE0 + m_unit);
	glBindTexture(GL_TEXTURE_2D, m_ID);
}

void Texture2D::unBind()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::open(const std::string & filename)
{
	
	const unsigned char* m_rawData = stbi_load(filename.c_str(), &m_width, &m_height, &m_components, 4);
	if (m_rawData == nullptr)
	{
		std::cout << "Unable to load texture: " << filename << std::endl;
		return;
	}
	else
		std::cout << "Texture: \"" << filename << "\" loaded succesfully" << std::endl;

	glEnable(GL_TEXTURE_2D);
	
	glGenTextures(1, &m_ID);
	glActiveTexture(GL_TEXTURE0 + m_unit);
	glBindTexture(GL_TEXTURE_2D, m_ID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_rawData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
}
