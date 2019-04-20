
#include "json_model_loader.h"
#include "../graphics/3d/mesh.h"
#include "../graphics/3d/model.h"
#include "../graphics/3d/vert_attributes.h"
#include "../files/file.h"
using json = nlohmann::json;

std::vector<SharedModel> JsonModelLoader::fromJsonFile(const char *path)
{
    JsonModelLoader loader(json::parse(File::readString(path)), path);
    return loader.models;
}

std::vector<SharedModel> JsonModelLoader::fromUbjsonFile(const char *path)
{
    JsonModelLoader loader(json::from_ubjson(File::readBinary(path)), path);
    return loader.models;
}

JsonModelLoader::JsonModelLoader(const json &obj, std::string id)
: obj(obj), id(id)
{   
    loadMeshes();
    loadModels();
}

void JsonModelLoader::loadMeshes()
{
    if (!obj.contains("meshes")) return;

    for (json meshJson : obj["meshes"])
    {
        VertAttributes attrs;
        for (std::string attrStr : meshJson["attributes"])
            attrs.add(attrFromString(attrStr));

        json info = meshJson["parts"][0];
        json indicesJson = info["indices"];
        json verticesJson = meshJson["vertices"];

        SharedMesh mesh = SharedMesh(new Mesh(info["id"], verticesJson.size(), indicesJson.size(), attrs));

        unsigned int i = 0;
        for (unsigned short index : indicesJson)
        {
            mesh->indices[i] = index;
            i++;
        }
        i = 0;
        for (float vert : verticesJson)
        {
            mesh->vertices[i] = vert;
            i++;
        }
        meshes.push_back(mesh);
    }
}

VertAttr JsonModelLoader::attrFromString(std::string str)
{
    if (str == "POSITION")
        return VertAttributes::POSITION;
    if (str == "NORMAL")
        return VertAttributes::NORMAL;
    else throw std::runtime_error("Vertex Attribute " + str + " not recognized. (" + id + ")");
}

void JsonModelLoader::loadModels()
{
    if (!obj.contains("nodes")) return;

    for (json modelJson : obj["nodes"])
    {
        if (!modelJson.contains("parts")) continue;

        SharedModel model = SharedModel(new Model(modelJson["id"]));

        for (json partJson : modelJson["parts"])
        {
            if (!partJson.contains("meshpartid")) continue;

            SharedMesh mesh;

            for (SharedMesh m : meshes) if (m->name == partJson["meshpartid"]) mesh = m;

            model->parts.push_back({mesh});
        }

        models.push_back(model);
    }
}
