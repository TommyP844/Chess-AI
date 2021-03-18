#pragma once
#include "shader.h"


Shader::Shader(const std::string & fragmentShader, const std::string & vertexShader, const std::string & geometryShader, const std::string & controlShader, const std::string & evaluationShader)
{
	load(fragmentShader, vertexShader, geometryShader, controlShader, evaluationShader);
}

void Shader::load(const std::string & fragmentShader, const std::string & vertexShader, const std::string & geometryShader, const std::string & controlShader, const std::string & evaluationShader)
{
	m_ID = glCreateProgram();
	m_ShaderSource[0] = fragmentShader;
	m_ShaderSource[1] = vertexShader;
	m_ShaderSource[2] = geometryShader;
	m_ShaderSource[3] = controlShader;
	m_ShaderSource[4] = evaluationShader;

	int FragmentID, VertexID, GeometryID, TessControlID, TessEvaluationID;

	FragmentID = compileSource(loadSource(fragmentShader), GL_FRAGMENT_SHADER);
	VertexID = compileSource(loadSource(vertexShader), GL_VERTEX_SHADER);
	if (!geometryShader.empty())
		GeometryID = compileSource(loadSource(geometryShader), GL_GEOMETRY_SHADER);
	if (!controlShader.empty() && !evaluationShader.empty())
	{
		TessControlID = compileSource(loadSource(controlShader), GL_TESS_CONTROL_SHADER);
		TessEvaluationID = compileSource(loadSource(evaluationShader), GL_TESS_EVALUATION_SHADER);
	}

	glAttachShader(m_ID, FragmentID);
	glAttachShader(m_ID, VertexID);

	if (!controlShader.empty() && !evaluationShader.empty())
	{
		glAttachShader(m_ID, TessControlID);
		glAttachShader(m_ID, TessEvaluationID);
	}
	if (!geometryShader.empty())
		glAttachShader(m_ID, GeometryID);
	
	glLinkProgram(m_ID);
	glValidateProgram(m_ID);

	glUseProgram(m_ID);
	checkCompileError(m_ID, GL_PROGRAM);


	glDeleteShader(VertexID);
	glDeleteShader(FragmentID);

	if (!geometryShader.empty())
		glDeleteShader(GeometryID);
	if (!controlShader.empty() && !evaluationShader.empty())
	{
		glDeleteShader(TessControlID);
		glDeleteShader(TessEvaluationID);
	}
	glUseProgram(0);
}

void Shader::bind()
{
	glUseProgram(m_ID);
}

void Shader::unbind()
{
	glUseProgram(0);
}

int Shader::getUniformLocation(const char* uniform)
{
	int location = glGetUniformLocation(m_ID, uniform);
	if (location == -1)
	{
		std::cout << "Invalid Location: " << uniform << " location: " << location << std::endl;
		std::cout << "Program: " << m_ID << std::endl;
		return 0;
	}
	std::cout << "Location: " << uniform << " Loaded." << std::endl;
	return location;

}

Shader::~Shader()
{
}

void Shader::setInt(int location, int value)
{
	glUniform1i(location, value);
}

void Shader::setFloat(int location, float value)
{
	glUniform1f(location, value);
}

void Shader::setVec2(int location, const glm::vec2& value)
{
	glUniform2fv(location, 1, &value[0]);
}

void Shader::setVec3(int location, const glm::vec3& value)
{
	glUniform3fv(location, 1, &value[0]);
}

void Shader::setVec4(int location, const glm::vec4& value)
{
	glUniform4fv(location, 1, &value[0]);
}

void Shader::setMat2(int location, const glm::mat2& value)
{
	glUniformMatrix2fv(location, 1, GL_FALSE, &value[0][0]);
}

void Shader::setMat3(int location, const glm::mat3& value)
{
	glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]);
}

void Shader::setMat4(int location, const glm::mat4& value)
{
	glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
}

void Shader::checkCompileError(unsigned int ID, int shadertype)
{
	std::string shader;
	switch (shadertype)
	{
	case GL_FRAGMENT_SHADER: shader = "Fragment"; break;
	case GL_VERTEX_SHADER: shader = "Vertex"; break;
	case GL_GEOMETRY_SHADER: shader = "Geometry"; break;
	case GL_TESS_CONTROL_SHADER: shader = "Tesselation Control"; break;
	case GL_TESS_EVALUATION_SHADER: shader = "Tesselation Evaluation"; break;
	case GL_COMPUTE_SHADER: shader = "Compute"; break;
	}

	char errorMsg[512];
	int success;
	if (shadertype == GL_PROGRAM)
	{
		glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
		if (!success)
		{
			std::cout << "Error Compiling Program. ID: " << m_ID << std::endl;
			glUseProgram(m_ID);
			char error[1024] = { 0 };

			glGetProgramInfoLog(m_ID, 1024, NULL, error);

			std::cout << error << std::endl;
		}
		else
			std::cout << "Shader Program Successfully compiled. ID: " << m_ID << std::endl;
	}
	else
	{
		glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(ID, 512, NULL, errorMsg);
			std::cout << "Filename: " << m_filename << std::endl;
			std::cout << "Error Compiling " + shader + ": " << errorMsg << std::endl;
		}
		else
			std::cout << shader + " compiled succesfully. ID: " << ID << std::endl;
	}
}

int Shader::compileSource(const std::string& source, int GLShader)
{
	int ShaderID = glCreateShader(GLShader);
	const char* buffer = source.c_str();
	int size = static_cast<GLint>(source.length());
	glShaderSource(ShaderID, 1, (char**)&buffer, &size);
	glCompileShader(ShaderID);

	checkCompileError(ShaderID, GLShader);

	return ShaderID;
}

std::string Shader::loadSource(const std::string& file)
{
	std::ifstream shaderFile;
	shaderFile.open(file.c_str());

	std::string output;
	std::string line;

	if (shaderFile.is_open())
	{
		while (shaderFile.good())
		{
			getline(shaderFile, line);
			output.append(line + "\n");
		}
	}
	else
		std::cerr << "Unable to load shader: " << file << std::endl;

	shaderFile.close();

	return output;
}
