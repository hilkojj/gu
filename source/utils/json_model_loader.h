
#ifndef MESH_LOADER_H
#define MESH_LOADER_H

#include "../json.hpp"
#include "../graphics/3d/mesh.h"
#include "../graphics/3d/model.h"
#include "../graphics/3d/vert_attributes.h"

/**
 * loads Models that are exported from Blender with https://github.com/hilkojj/blender_UBJSON_exporter
 **/
class JsonModelLoader
{
  public:
    // predefinedAttrs should be NULL unless you want to define the vertex layout yourself.
    static std::vector<SharedModel> fromJsonFile(const char *path, VertAttributes *predefinedAttrs);
    // predefinedAttrs should be NULL unless you want to define the vertex layout yourself.
    static std::vector<SharedModel> fromUbjsonFile(const char *path, VertAttributes *predefinedAttrs);

    // note: 'id' is only used for error messages, predefinedAttrs should be NULL unless you want to define the vertex layout yourself.
    JsonModelLoader(const json &obj, std::string id, VertAttributes *predefinedAttrs);

    std::vector<SharedModel> models;
    std::vector<SharedMesh> meshes;

  private:
    
    VertAttributes *predefinedAttrs;
    const json &obj;
    std::string id;

    void loadMeshes();

    void loadModels();

    VertAttr attrFromString(std::string str);

};

#endif
