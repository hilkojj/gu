
#include "glad/glad.h"

#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

class ShaderProgram
{
  public:
    static ShaderProgram fromFiles(std::string name, const char *vertPath, const char *fragPath);

    ShaderProgram(std::string name, const char *vertSource, const char *fragSource);
    ~ShaderProgram();

    bool compiled() const;

  private:
    GLuint programId;
    std::string name;
    bool compiled_;

    void compileAndAttach(const char *source, GLuint shaderId);
};

#endif
