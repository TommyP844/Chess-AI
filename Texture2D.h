#pragma once


// API's
#include <GL\glew.h>

// Standard Headers
#include <string>
#include <iostream>
#include <vector>



class Texture2D
{
public:
	Texture2D(const std::string& filename, int unit = 0);
	Texture2D() {}
	
	void load(const std::string& filename, int unit = 0);

	void release();

	void setUnit(int unit);
	void bind();
	void unBind();

	int getUnit() const;
	int getWidth() const;
	int getHeight() const;
	unsigned int getID() const;



private:
	void open(const std::string& filename);
	
	unsigned int m_ID;

	int m_width;
	int m_height;
	int m_components;

	int m_unit;
	
};
