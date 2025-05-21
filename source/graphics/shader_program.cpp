#include "shader_program.h"

#include "external/gl_includes.h"

#include "../files/file_utils.h"
#include "../utils/string_utils.h"
#include "../utils/gu_error.h"

#include <iostream>

ShaderProgram::ShaderProgram(const std::string &name, const char *vertSource, const char *fragSource, bool compile)
    : name(name)
{
    if (compile)
        this->compile(vertSource, fragSource);
}

ShaderProgram::ShaderProgram(const std::string &name, const char *vertSource, const char *geomSource, const char *fragSource, bool compile)
    : name(name)
{
    if (compile)
        this->compile(vertSource, fragSource, geomSource);
}

int realSourceCodeStartsAt = 0;

void ShaderProgram::compile(const char *vertSource, const char *fragSource, const char *geomSource)
{
    #ifdef EMSCRIPTEN
    geomSource = NULL;
    #endif

    programId = glCreateProgram();
    GLuint vertShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShaderId = fragSource ? glCreateShader(GL_FRAGMENT_SHADER) : 0;
    GLuint geomShaderId = geomSource ? glCreateShader(GL_GEOMETRY_SHADER) : 0;

    compileAndAttach(vertSource, vertShaderId, "VERTEX");
    if (fragSource)
    {
        compileAndAttach(fragSource, fragShaderId, "FRAGMENT");
    }
    if (geomSource)
    {
        compileAndAttach(geomSource, geomShaderId, "GEOMETRY");
    }
    if (!feedbackVaryings.empty())
    {
        glTransformFeedbackVaryings(programId, feedbackVaryings.size(), feedbackVaryings.data(), GL_INTERLEAVED_ATTRIBS);
    }
    glLinkProgram(programId);

    // check program:
    int logLength;
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 1)
    {
        std::vector<char> log(logLength + 1);
        glGetProgramInfoLog(programId, logLength, NULL, &log[0]);
        std::cout << name << " compilation log:\n" << &log[0] << std::endl;
    }

    glDetachShader(programId, vertShaderId);
    if (fragSource)
    {
        glDetachShader(programId, fragShaderId);
    }
    if (geomSource)
    {
        glDetachShader(programId, geomShaderId);
    }

    glDeleteShader(vertShaderId);
    if (fragSource)
    {
        glDeleteShader(fragShaderId);
    }
    if (geomSource)
    {
        glDeleteShader(geomShaderId);
    }

    compiled_ = true;
    compileFinishTime = glfwGetTime();
}

void ShaderProgram::compileAndAttach(const char *source, GLuint shaderId, const char *shaderType)
{
    auto versionStr = ShaderDefinitions::global().getVersionLine();
    auto definitions = ShaderDefinitions::global().getGLSLString() + this->definitions.getGLSLString() + "#define " + shaderType + "_SHADER\n#line 1\n";
    realSourceCodeStartsAt = su::getNumNewlines(versionStr) + su::getNumNewlines(definitions) + 2;

    const GLchar *sources[] = {versionStr.c_str(), definitions.c_str(), source};

    glShaderSource(shaderId, 3, sources, NULL);
    glCompileShader(shaderId);
    glAttachShader(programId, shaderId);

    #ifdef EMSCRIPTEN
    // check shader:
    int logLength;
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

GLint ShaderProgram::location(const char *uniformName) const
{
    return glGetUniformLocation(programId, uniformName);
}

void ShaderProgram::use()
{
    if (!compiled_)
        throw gu_err("This shader is not compiled!");
    glUseProgram(programId);
}

void ShaderProgram::setFeedbackVaryings(const std::vector<const char *> &varyings)
{
    feedbackVaryings = varyings;
}

ShaderProgram ShaderProgram::fromFiles(const std::string &name, const std::string& vertPath, const std::string& fragPath)
{
    const std::string vertCode = fu::readString(vertPath.c_str());
    const std::string fragCode = fu::readString(fragPath.c_str());
    return ShaderProgram(name, vertCode.c_str(), fragCode.c_str());
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(programId);
}

ShaderProgram ShaderProgram::fromFiles(const std::string &name, const std::string &vertPath, const std::string &geomPath,
    const std::string &fragPath)
{
    const std::string vertCode = fu::readString(vertPath.c_str());
    const std::string geomCode = fu::readString(geomPath.c_str());
    const std::string fragCode = fu::readString(fragPath.c_str());
    return ShaderProgram(name, vertCode.c_str(), geomCode.c_str(), fragCode.c_str());
}

std::string ShaderDefinitions::getGLSLString() const
{
    std::string str;

    for (auto &[key, val] : definitionsMap)
        str += "#define " + key + " " + val + "\n";

    return str;
}

bool ShaderDefinitions::isDefined(const std::string &key)
{
    return definitionsMap.find(key) != definitionsMap.end();
}

void ShaderDefinitions::define(const char *name, std::string val)
{
    definitionsMap[name] = std::move(val);
    lastEditTime = glfwGetTime();
}

void ShaderDefinitions::undef(const char *name)
{
    definitionsMap.erase(name);
    lastEditTime = glfwGetTime();
}

void ShaderDefinitions::defineInt(const char *name, int val)
{
    define(name, std::to_string(val));
}

void ShaderDefinitions::defineUint(const char *name, unsigned int val)
{
    define(name, std::to_string(val) + "u");
}

void ShaderDefinitions::defineFloat(const char *name, float val)
{
    define(name, std::to_string(val) + "f");
}

void ShaderDefinitions::defineDouble(const char *name, double val)
{
    define(name, std::to_string(val));
}

ShaderDefinitions &ShaderDefinitions::global()
{
    static ShaderDefinitions global;
    return global;
}

std::string &ShaderDefinitions::getVersionLine()
{
    #ifdef EMSCRIPTEN
    static std::string str = "#version 300 es\n";
    #else
    static std::string str = "#version 400\n";
    #endif // EMSCRIPTEN
    return str;
}
