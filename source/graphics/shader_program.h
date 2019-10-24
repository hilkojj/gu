
#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include <string>
#include "../gl_includes.h"

class ShaderProgram
{
  public:
    static bool reloadFromFile;
    static ShaderProgram fromFiles(std::string name, const std::string& vertPath, const std::string& fragPath);

    ShaderProgram(std::string name, const char *vertSource, const char *fragSource, std::string vertPath="", std::string fragPath="");
    ~ShaderProgram();

    bool compiled() const;
    GLuint id() const;
    GLuint location(const char *uniformName) const;

    void use();

  private:
    GLuint programId;
    std::string name, vertPath, fragPath;
    bool compiled_, reloadedFromFile = false;

    void compile(const char *vertSource, const char *fragSource);

    void compileAndAttach(const char *source, GLuint shaderId, const char *shaderType);
};

#endif
