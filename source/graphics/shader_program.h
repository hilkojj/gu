
#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include <string>
#include <map>
#include "../gl_includes.h"

class ShaderDefinitions
{

    std::map<std::string, std::string> definitionsMap;
    double lastEditTime = 0;
    friend class ShaderAsset;

public:

    static ShaderDefinitions &global();

    static std::string &getVersionLine();

    std::string getGLSLString() const;

    bool isDefined(const std::string &);

    void define(const char *name, std::string val = "");
    void undef(const char *name);
    void defineInt(const char *name, int val);
    void defineFloat(const char *name, float val);
    void defineDouble(const char *name, double val);

};

class ShaderProgram
{
  public:
    ShaderDefinitions definitions;

    static ShaderProgram fromFiles(std::string name, const std::string& vertPath, const std::string& fragPath);

    static ShaderProgram fromFiles(
            std::string name, const std::string& vertPath, const std::string& geomPath, const std::string& fragPath
    );

    ShaderProgram(std::string name, const char *vertSource, const char *fragSource, bool compile=true);

    ShaderProgram(
            std::string name, const char *vertSource, const char *geomSource, const char *fragSource, bool compile=true
    );

    ~ShaderProgram();

    bool compiled() const;
    GLuint id() const;
    GLuint location(const char *uniformName) const;

    virtual void use();

  protected:
    GLuint programId = 0;
    std::string name;
    bool compiled_ = false;

    double compileFinishTime = 0;

    void compile(const char *vertSource, const char *fragSource, const char *geomSource=NULL);

    void compileAndAttach(const char *source, GLuint shaderId, const char *shaderType);
};

#endif
