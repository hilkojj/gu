
#include <iostream>
#include <vector>

#include "shader_program.h"
#include "../files/file.h"

bool ShaderProgram::reloadFromFile = false;

ShaderProgram::ShaderProgram(std::string name, const char *vertSource, const char *fragSource, std::string vertPath, std::string fragPath)

    : name(name), vertPath(vertPath), fragPath(fragPath)
{
    compile(vertSource, fragSource);
}

void ShaderProgram::compile(const char *vertSource, const char *fragSource)
{
    programId = glCreateProgram();
    GLuint vertShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    compileAndAttach(vertSource, vertShaderId, "VERTEX");
    compileAndAttach(fragSource, fragShaderId, "FRAGMENT");

    glLinkProgram(programId);

    // check program:
    GLint success = GL_FALSE;
    int logLength;
    glGetProgramiv(programId, GL_COMPILE_STATUS, &success);
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 1)
    {
        std::vector<char> log(logLength + 1);
        glGetProgramInfoLog(programId, logLength, NULL, &log[0]);
        std::cout << name << " compilation log:\n"
                    << &log[0] << "\n";
    }

    glDetachShader(programId, vertShaderId);
    glDetachShader(programId, fragShaderId);

    glDeleteShader(vertShaderId);
    glDeleteShader(fragShaderId);

    compiled_ = success;
}

void ShaderProgram::compileAndAttach(const char *source, GLuint shaderId, const char *shaderType)
{
    glShaderSource(shaderId, 1, &source, NULL);
    glCompileShader(shaderId);
    glAttachShader(programId, shaderId);

    #ifdef EMSCRIPTEN
    // check shader:
    GLint success = GL_FALSE;
    int logLength;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 1)
    {
        std::vector<char> log(logLength + 1);
        glGetShaderInfoLog(shaderId, logLength, NULL, &log[0]);
        std::cout << name << " " << shaderType <<  " SHADER compilation log:\n"
                    << &log[0] << "\n";
    }
    #endif
}

bool ShaderProgram::compiled() const
{
    return compiled_;
}

GLuint ShaderProgram::id() const
{
    return programId;
}

GLuint ShaderProgram::location(const char *uniformName) const
{
    return glGetUniformLocation(programId, uniformName);
}

void ShaderProgram::use()
{
    if (reloadFromFile)
    {
        if (!reloadedFromFile && vertPath.size())
        {
            std::string vertCode = File::readString(vertPath.c_str());
            std::string fragCode = File::readString(fragPath.c_str());
            glDeleteProgram(programId);
            compile(vertCode.c_str(), fragCode.c_str());
        }
        reloadedFromFile = true;
    }
    else reloadedFromFile = false;
    
    glUseProgram(programId);
}

ShaderProgram ShaderProgram::fromFiles(std::string name, const std::string& vertPath, const std::string& fragPath)
{
    std::string vertCode = File::readString(vertPath.c_str());
    std::string fragCode = File::readString(fragPath.c_str());
    return ShaderProgram(std::move(name), vertCode.c_str(), fragCode.c_str(), vertPath, fragPath);
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(programId);
}
