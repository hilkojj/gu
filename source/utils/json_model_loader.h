
#ifndef MESH_LOADER_H
#define MESH_LOADER_H

#include "../../external/json.hpp"
#include "../graphics/3d/mesh.h"
#include "../graphics/3d/model.h"
using json = nlohmann::json;

/**
 * loads Models that are exported from Blender with https://github.com/hilkojj/blender_UBJSON_exporter
 **/
class JsonModelLoader
{

  public:
    static std::vector<SharedModel> fromJsonFile(const char *path);
    static std::vector<SharedModel> fromUbjsonFile(const char *path);

    // note: 'id' is only used for error messages
    JsonModelLoader(const json &obj, std::string id);

    std::vector<SharedModel> models;
    std::vector<SharedMesh> meshes;

  private:
    
    const json &obj;
    std::string id;

    void loadMeshes();

    void loadModels();

    VertAttr attrFromString(std::string str);

};

#endif
