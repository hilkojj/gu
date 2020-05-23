#include <iostream>
#include <utility>
#include <vector>

#include "shader_program.h"
#include "../files/file.h"

ShaderProgram::ShaderProgram(std::string name, const char *vertSource, const char *fragSource)

    : name(std::move(name))
{
    compile(vertSource, fragSource);
}

ShaderProgram::ShaderProgram(std::string name, const char *vertSource, const char *geomSource, const char *fragSource)
    : name(std::move(name))
{
    compile(vertSource, fragSource, geomSource);
}

void ShaderProgram::compile(const char *vertSource, const char *fragSource, const char *geomSource)
{
    programId = glCreateProgram();
    GLuint vertShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    GLuint geomShaderId = geomSource ? glCreateShader(GL_GEOMETRY_SHADER) : 0;

    compileAndAttach(vertSource, vertShaderId, "VERTEX");
    compileAndAttach(fragSource, fragShaderId, "FRAGMENT");
    if (geomSource)
        compileAndAttach(geomSource, geomShaderId, "GEOMETRY");

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
                    << &log[0] << std::endl;
    }

    glDetachShader(programId, vertShaderId);
    glDetachShader(programId, fragShaderId);
    if (geomSource)
        glDetachShader(programId, geomShaderId);

    glDeleteShader(vertShaderId);
    glDeleteShader(fragShaderId);
    if (geomSource)
        glDeleteShader(geomShaderId);

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
    glUseProgram(programId);
}

ShaderProgram ShaderProgram::fromFiles(std::string name, const std::string& vertPath, const std::string& fragPath)
{
    std::string vertCode = File::readString(vertPath.c_str());
    std::string fragCode = File::readString(fragPath.c_str());
    return ShaderProgram(std::move(name), vertCode.c_str(), fragCode.c_str());
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(programId);
}

ShaderProgram ShaderProgram::fromFiles(std::string name, const std::string &vertPath, const std::string &geomPath,
                                       const std::string &fragPath)
{
    std::string vertCode = File::readString(vertPath.c_str());
    std::string geomCode = File::readString(geomPath.c_str());
    std::string fragCode = File::readString(fragPath.c_str());
    return ShaderProgram(std::move(name), vertCode.c_str(), geomCode.c_str(), fragCode.c_str());
}
