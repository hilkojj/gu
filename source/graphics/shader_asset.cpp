#include "shader_asset.h"
#include "external/gl_includes.h"

void ShaderAsset::use()
{
    if (vertCode.hasReloaded()
        || (fragCode.isSet() && fragCode.hasReloaded())
        || (geomCode.isSet() && geomCode.hasReloaded())
        || compileFinishTime < ShaderDefinitions::global().lastEditTime || compileFinishTime < definitions.lastEditTime
        || !compiled_)
    {
        glDeleteProgram(programId);
        compile(
            vertCode->c_str(),
            fragCode.isSet() ? fragCode->c_str() : nullptr,
            geomCode.isSet() ? geomCode->c_str() : nullptr
        );
    }
    ShaderProgram::use();
}
