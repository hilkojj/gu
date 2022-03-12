#include <iostream>
#include <utility>
#include <vector>

#include "shader_program.h"
#include "../files/file.h"
#include "../utils/string.h"

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

int realSourceCodeStartsAt = 0;

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
        std::cout << name << " compilation log:\n" << "(note: line count starts at " << realSourceCodeStartsAt << ")\n"
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
    compileFinishTime = glfwGetTime();
}

void ShaderProgram::compileAndAttach(const char *source, GLuint shaderId, const char *shaderType)
{
    auto versionStr = ShaderDefinitions::getVersionLine();
    auto definitions = ShaderDefinitions::getGLSLString();
    realSourceCodeStartsAt = nrOfNewlines(versionStr) + nrOfNewlines(definitions) + 2;

    const GLchar *sources[] = {versionStr.c_str(), definitions.c_str(), source};

    glShaderSource(shaderId, 3, sources, NULL);
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
                    << "(note: line count starts at " << realSourceCodeStartsAt << ")\n"
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

std::map<std::string, std::string> &ShaderDefinitions::getDefinitionsMap()
{
    static std::map<std::string, std::string> map;
    return map;
}

std::string ShaderDefinitions::getGLSLString()
{
    std::string str;

    for (auto &[key, val] : getDefinitionsMap())
        str += "#define " + key + " " + val + "\n";

    return str;
}

void ShaderDefinitions::define(const char *name, std::string val)
{
    getDefinitionsMap()[name] = std::move(val);
    getLastEditTime() = glfwGetTime();
}

void ShaderDefinitions::undef(const char *name)
{
    getDefinitionsMap().erase(name);
}

void ShaderDefinitions::defineInt(const char *name, int val)
{
    define(name, std::to_string(val));
}

void ShaderDefinitions::defineFloat(const char *name, float val)
{
    define(name, std::to_string(val) + "f");
}

void ShaderDefinitions::defineDouble(const char *name, double val)
{
    define(name, std::to_string(val));
}

double &ShaderDefinitions::getLastEditTime()
{
    static double lastEditTime = glfwGetTime();
    return lastEditTime;
}

std::string &ShaderDefinitions::getVersionLine()
{
    static std::string str = "#version 400\n";
    return str;
}
