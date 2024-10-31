#pragma once

#ifndef GU_PBR_ONLY

#include "../mesh.h"
#include "../model.h"
#include "../vert_attributes.h"

#include "../../../json_fwd.hpp"

/**
 * loads Models that are exported from Blender with https://github.com/hilkojj/blender_UBJSON_exporter
 *
 * Format Specification:
 * https://github.com/libgdx/fbx-conv/wiki/Version-0.1-%28libgdx-0.9.9%29
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

    std::vector<SharedArmature> armatures;
    std::vector<SharedMaterial> materials;
    std::vector<SharedModel> models;
    std::vector<SharedMesh> meshes;

  private:

    const VertAttributes *predefinedAttrs;
    const json *obj;
    std::string id, textureBasePath;

    void loadMaterials();

    void loadMeshes();

    void loadModels();

    void loadArmatures();

    VertAttr attrFromString(const std::string &str);

};

#endif
