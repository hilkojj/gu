
#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include <string>
#include "../gl_includes.h"

class ShaderProgram
{
  public:
    static bool reloadFromFile;
    static ShaderProgram fromFiles(std::string name, const std::string& vertPath, const std::string& fragPath);

    static ShaderProgram fromFiles(
            std::string name, const std::string& vertPath, const std::string& geomPath, const std::string& fragPath
    );

    ShaderProgram(std::string name, const char *vertSource, const char *fragSource, std::string vertPath="", std::string fragPath="");

    ShaderProgram(
            std::string name, const char *vertSource, const char *geomSource, const char *fragSource, std::string vertPath="", std::string geomPath="", std::string fragPath=""
    );

    ~ShaderProgram();

    bool compiled() const;
    GLuint id() const;
    GLuint location(const char *uniformName) const;

    void use();

  private:
    GLuint programId;
    std::string name, vertPath, fragPath, geomPath;
    bool compiled_, reloadedFromFile = false;

    void compile(const char *vertSource, const char *fragSource, const char *geomSource=NULL);

    void compileAndAttach(const char *source, GLuint shaderId, const char *shaderType);
};

#endif
