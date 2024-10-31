#pragma once

#include <string>
#include <map>
#include <vector>

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
    void defineUint(const char *name, unsigned int val);
    void defineFloat(const char *name, float val);
    void defineDouble(const char *name, double val);

};

class ShaderProgram
{
  public:
    ShaderDefinitions definitions;

    static ShaderProgram fromFiles(const std::string &name, const std::string& vertPath, const std::string& fragPath);

    static ShaderProgram fromFiles(
        const std::string &name, const std::string& vertPath, const std::string& geomPath, const std::string& fragPath
    );

    ShaderProgram(const std::string &name, const char *vertSource, const char *fragSource, bool compile=true);

    ShaderProgram(
        const std::string &name, const char *vertSource, const char *geomSource, const char *fragSource, bool compile=true
    );

    ~ShaderProgram();

    bool compiled() const;
    unsigned int id() const;
    int location(const char *uniformName) const;

    virtual void use();

    // Set varyings to load from the program into C++. Must be set before compiling.
    void setFeedbackVaryings(const std::vector<const char *> &varyings);

  protected:
    unsigned int programId = 0;
    std::string name;
    bool compiled_ = false;

    double compileFinishTime = 0;

    std::vector<const char *> feedbackVaryings;

    void compile(const char *vertSource, const char *fragSource, const char *geomSource=NULL);

    void compileAndAttach(const char *source, unsigned int shaderId, const char *shaderType);
};
