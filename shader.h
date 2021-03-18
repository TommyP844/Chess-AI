#pragma once

// Standard Headers
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

// API's
#include <GL/glew.h>
#include <GL/GL.h>
#include <glm\gtc\type_ptr.hpp>
#include <array>

#include "Texture2D.h"



class Shader
{
public:
	enum ShaderType
	{
		ShaderType_FRAGMENT,
		ShaderType_VERTEX,
		ShaderType_GEOMETRY,
		ShaderType_TESSELATION_CONTROL,
		ShaderType_TESSELATION_EVALUATION
	};


	Shader(const std::string& fragmentShader, const std::string& vertexShader,
		const std::string& geometryShader = std::string(), 
		const std::string& controlShader = std::string(),
		const std::string& evaluationShader = std::string());
		
	void load(const std::string& fragmentShader, const std::string& vertexShader,
		const std::string& geometryShader = std::string(),
		const std::string& controlShader = std::string(),
		const std::string& evaluationShader = std::string());

	Shader() {}

	void bind();

	void unbind();

	int getUniformLocation(const char* uniform);
	
	virtual ~Shader();

	void     setInt(int location, int value);
	void   setFloat(int location, float value);
	void    setVec2(int location, const glm::vec2& value);
	void    setVec3(int location, const glm::vec3& value);
	void    setVec4(int location, const glm::vec4& value);
	void    setMat2(int location, const glm::mat2& value);
	void    setMat3(int location, const glm::mat3& value);
	void    setMat4(int location, const glm::mat4& value);

private:
	unsigned int m_ID;
	std::array<std::string, 5> m_ShaderSource;
	std::string m_filename;

	void checkCompileError(unsigned int ID, int shadertype);
	int compileSource(const std::string& source, int GLShader);
	std::string loadSource(const std::string& file);

};


