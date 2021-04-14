
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
    static std::vector<SharedModel> fromJsonFile(const char *path, const VertAttributes *predefinedAttrs=NULL);
    // predefinedAttrs should be NULL unless you want to define the vertex layout yourself.
    static std::vector<SharedModel> fromUbjsonFile(const char *path, const VertAttributes *predefinedAttrs=NULL);

    // note: 'id' is only used for error messages, predefinedAttrs should be NULL unless you want to define the vertex layout yourself.
    JsonModelLoader(const json &obj, std::string id, const VertAttributes *predefinedAttrs, std::string textureBasePath);

    std::vector<SharedMaterial> materials;
    std::vector<SharedModel> models;
    std::vector<SharedMesh> meshes;

  private:
    
    const VertAttributes *predefinedAttrs;
    const json &obj;
    std::string id, textureBasePath;

    void loadMaterials();

    void loadMeshes();

    void loadModels();

    VertAttr attrFromString(const std::string &str);

};

#endif
