
#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include <string>
#include <map>
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

    double compileFinishTime = 0;

    void compile(const char *vertSource, const char *fragSource, const char *geomSource=NULL);

    void compileAndAttach(const char *source, GLuint shaderId, const char *shaderType);
};

class ShaderDefinitions
{

    static std::map<std::string, std::string> &getDefinitionsMap();
    static double &getLastEditTime();
    friend class ShaderAsset;

  public:

    static std::string getGLSLString();
    static std::string &getVersionLine();

    static void define(const char *name, std::string val="");
    static void undef(const char *name);
    static void defineInt(const char *name, int val);
    static void defineFloat(const char *name, float val);
    static void defineDouble(const char *name, double val);

};

#endif
