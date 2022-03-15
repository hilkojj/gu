
#ifndef GAME_SHADER_ASSET_H
#define GAME_SHADER_ASSET_H

#include "shader_program.h"
#include "../asset_manager/asset.h"

class ShaderAsset : public ShaderProgram
{

    asset<std::string> vertCode, fragCode, geomCode;

  public:

    ShaderAsset(const std::string &name, const std::string& vertPath, const std::string& fragPath, bool compile=true)
        : ShaderProgram(name, asset<std::string>(vertPath)->c_str(), asset<std::string>(fragPath)->c_str(), compile),
          vertCode(vertPath), fragCode(fragPath)
    {}

    ShaderAsset(const std::string &name, const std::string &vertPath, const std::string &geomPath, const std::string &fragPath, bool compile=true)
        : ShaderProgram(name, asset<std::string>(vertPath)->c_str(), asset<std::string>(geomPath)->c_str(), asset<std::string>(fragPath)->c_str(), compile),
        vertCode(vertPath), fragCode(fragPath), geomCode(geomPath)
    {}

    void use() override
    {
        if (vertCode.hasReloaded() || fragCode.hasReloaded() || (geomCode.isSet() && geomCode.hasReloaded())
            || compileFinishTime < ShaderDefinitions::global().lastEditTime || compileFinishTime < definitions.lastEditTime
            || !compiled_)
        {
            glDeleteProgram(programId);
            compile(vertCode->c_str(), fragCode->c_str(), geomCode.isSet() ? geomCode->c_str() : NULL);
        }
        ShaderProgram::use();
    }

};


#endif
