
#include "json_model_loader.h"
#include "../graphics/3d/mesh.h"
#include "../graphics/3d/model.h"
#include "../graphics/3d/vert_attributes.h"
#include "../files/file.h"
#include "gu_error.h"
using json = nlohmann::json;

std::vector<SharedModel> JsonModelLoader::fromJsonFile(const char *path, VertAttributes *predefinedAttrs)
{
    JsonModelLoader loader(json::parse(File::readString(path)), path, predefinedAttrs);
    return loader.models;
}

std::vector<SharedModel> JsonModelLoader::fromUbjsonFile(const char *path, VertAttributes *predefinedAttrs)
{
    JsonModelLoader loader(json::from_ubjson(File::readBinary(path)), path, predefinedAttrs);
    return loader.models;
}

JsonModelLoader::JsonModelLoader(const json &obj, std::string id, VertAttributes *predefinedAttrs)
: obj(obj), id(id), predefinedAttrs(predefinedAttrs)
{   
    loadMeshes();
    loadModels();
}

void JsonModelLoader::loadMeshes()
{
    if (!obj.contains("meshes")) return;

    for (json meshJson : obj["meshes"])
    {
        VertAttributes originalAttrs;
        for (std::string attrStr : meshJson["attributes"])
            originalAttrs.add(attrFromString(attrStr));

        json info = meshJson["parts"][0];
        json indicesJson = info["indices"];
        json verticesJson = meshJson["vertices"];

        SharedMesh mesh = SharedMesh(new Mesh(
            info["id"], 
            verticesJson.size() / originalAttrs.getVertSize(), 
            indicesJson.size(), 
            predefinedAttrs ? *predefinedAttrs : originalAttrs
        ));

        unsigned int i = 0;
        for (unsigned short index : indicesJson)
        {
            mesh->indices[i] = index;
            i++;
        }

        if (!predefinedAttrs)  // load vertices as laid out in JSON
        {
            i = 0;
            for (float vert : verticesJson)
            {
                mesh->vertices[i] = vert;
                i++;
            }
        }
        else                    // load vertices as laid out in predefinedAttrs
        {
            for (int i = 0; i < predefinedAttrs->nrOfAttributes(); i++)
            {
                VertAttr &attr = predefinedAttrs->get(i);
                if (!originalAttrs.contains(attr)) continue;

                int originalOffset = originalAttrs.getOffset(attr);
                int newOffset = predefinedAttrs->getOffset(attr);

                for (int v = 0; v < mesh->nrOfVertices; v++)
                {
                    for (int j = 0; j < attr.size; j++)
                    {
                        int originalIndex = originalOffset + j + v * originalAttrs.getVertSize();
                        int newIndex = newOffset + j + v * predefinedAttrs->getVertSize();
                        mesh->vertices[newIndex] = verticesJson[originalIndex];
                    }
                }
            }
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
    if (str == "TEXCOORD0")
        return VertAttributes::TEX_COORDS;
    else throw gu_err("Vertex Attribute " + str + " not recognized. (" + id + ")");
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
