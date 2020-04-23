
#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include <string>
#include "../gl_includes.h"

class ShaderProgram
{
  public:
    static ShaderProgram fromFiles(std::string name, const std::string& vertPath, const std::string& fragPath);

    static ShaderProgram fromFiles(
            std::string name, const std::string& vertPath, const std::string& geomPath, const std::string& fragPath
    );

    ShaderProgram(std::string name, const char *vertSource, const char *fragSource);

    ShaderProgram(
            std::string name, const char *vertSource, const char *geomSource, const char *fragSource
    );

    ~ShaderProgram();

    bool compiled() const;
    GLuint id() const;
    GLuint location(const char *uniformName) const;

    virtual void use();

  protected:
    GLuint programId;
    std::string name;
    bool compiled_;

    void compile(const char *vertSource, const char *fragSource, const char *geomSource=NULL);

    void compileAndAttach(const char *source, GLuint shaderId, const char *shaderType);
};

#endif
