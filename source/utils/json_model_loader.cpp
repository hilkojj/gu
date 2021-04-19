
#include "json_model_loader.h"
#include "../files/file.h"
#include "gu_error.h"

using json = nlohmann::json;

std::string basePath(const std::string &filePath)
{
    for (int i = filePath.size() - 1; i >= 0; i--)
        if (filePath[i] == '/' || filePath[i] == '\\')
            return filePath.substr(0, i + 1);
    return "";
}

std::vector<SharedModel> JsonModelLoader::fromJsonFile(const char *path, const VertAttributes *predefinedAttrs)
{
    JsonModelLoader loader(json::parse(File::readString(path)), path, predefinedAttrs, basePath(path));
    return loader.models;
}

std::vector<SharedModel> JsonModelLoader::fromUbjsonFile(const char *path, const VertAttributes *predefinedAttrs)
{
    JsonModelLoader loader(json::from_ubjson(File::readBinary(path)), path, predefinedAttrs, basePath(path));
    return loader.models;
}

JsonModelLoader::JsonModelLoader(const json &obj, std::string id, const VertAttributes *predefinedAttrs, std::string textureBasePath)
: obj(obj), id(id), predefinedAttrs(predefinedAttrs), textureBasePath(textureBasePath)
{
    loadMaterials();
    loadMeshes();
    loadModels();
}

void JsonModelLoader::loadMeshes()
{
    if (!obj.contains("meshes")) return;

    for (const json &meshJson : obj["meshes"])
    {
        VertAttributes originalAttrs;
        for (std::string attrStr : meshJson["attributes"])
            originalAttrs.add(attrFromString(attrStr));

        json info = meshJson["parts"].at(0);
        json verticesJson = meshJson["vertices"];


        SharedMesh mesh = SharedMesh(new Mesh(
            splitString(info["id"], "_part").at(0),
            verticesJson.size() / originalAttrs.getNrOfComponents(),
            predefinedAttrs ? *predefinedAttrs : originalAttrs
        ));

        for (const json &partJson : meshJson["parts"])
        {
            auto &part = mesh->parts.emplace_back();
            part.name = partJson["id"];

            for (unsigned short index : partJson["indices"])
                part.indices.push_back(index);
        }


        if (!predefinedAttrs)  // load vertices as laid out in JSON
        {
            unsigned int i = 0;
            for (float vert : verticesJson)
                mesh->set(vert, 0, i++ * sizeof(float));
        }
        else                    // load vertices as laid out in predefinedAttrs
        {
            for (int i = 0; i < predefinedAttrs->nrOfAttributes(); i++)
            {
                auto &attr = predefinedAttrs->get(i);
                if (!originalAttrs.contains(attr)) continue;
                if (attr.type != GL_FLOAT) throw gu_err("Tried to load a non-float attribute from a JSON model");

                int originalOffset = originalAttrs.getOffset(attr) / sizeof(float);
                int newOffset = predefinedAttrs->getOffset(attr);

                for (int v = 0; v < mesh->nrOfVertices(); v++)
                {
                    for (int j = 0; j < attr.size; j++)
                    {
                        int originalIndex = originalOffset + j + v * originalAttrs.getVertSize() / sizeof(float);
                        mesh->set<float>(verticesJson[originalIndex], v, newOffset + j * sizeof(float));
                    }
                }
            }
        }
        meshes.push_back(mesh);
    }
}

VertAttr JsonModelLoader::attrFromString(const std::string &str)
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

    for (const json &modelJson : obj["nodes"])
    {
        if (!modelJson.contains("parts")) continue;

        SharedModel model = SharedModel(new Model(modelJson["id"]));

        for (const json &partJson : modelJson["parts"])
        {
            auto &modelPart = model->parts.emplace_back();

            if (partJson.contains("meshpartid"))
            {
                std::string meshpartid = partJson["meshpartid"];
                for (SharedMesh &m : meshes)
                    for (int i = 0; i < m->parts.size(); i++)
                        if (m->parts[i].name == meshpartid)
                        {
                            modelPart.mesh = m;
                            modelPart.meshPartIndex = i;
                        }
                if (modelPart.mesh == NULL)
                    throw gu_err("Model (" + model->name + ") wants a mesh part named " + meshpartid + " but that part could not be found");
            }

            for (SharedMaterial &m : materials)
                if (m->name == partJson["materialid"]) modelPart.material = m;  // todo, also give error when material was not found.
        }

        models.push_back(model);
    }
}

void JsonModelLoader::loadMaterials()
{
    if (!obj.contains("materials")) return;

    for (const json &matJson : obj["materials"])
    {
        SharedMaterial mat = std::make_shared<Material>();
        mat->name = matJson.contains("id") ? matJson["id"] : "untitled";

        mat->ambient = matJson.contains("ambient") ? vec3(matJson["ambient"][0], matJson["ambient"][1], matJson["ambient"][2]) : vec3(0);
        mat->diffuse = matJson.contains("diffuse") ? vec3(matJson["diffuse"][0], matJson["diffuse"][1], matJson["diffuse"][2]) : vec3(0);
        mat->emissive = matJson.contains("emissive") ? vec3(matJson["emissive"][0], matJson["emissive"][1], matJson["emissive"][2]) : vec3(0);
        mat->reflection = matJson.contains("reflection") ? vec3(matJson["reflection"][0], matJson["reflection"][1], matJson["reflection"][2]) : vec3(0);
        mat->specular = matJson.contains("specular") ? vec4(matJson["specular"][0], matJson["specular"][1], matJson["specular"][2], matJson["specular"][3]) : vec4(0);
        mat->shininess = matJson.contains("shininess") ? float(matJson["shininess"]) : 0.f;

        if (matJson.contains("textures"))
        {
            for (const json &texJson : matJson["textures"])
            {
                asset<Texture> tex((textureBasePath + std::string(texJson.at("filename"))));

                if (texJson.at("type") == "DIFFUSE")
                    mat->diffuseTexture = tex;
                else if (texJson.at("type") == "SPECULAR")
                    mat->specularMap = tex;
                else if (texJson.at("type") == "BUMP")
                    mat->normalMap = tex;
            }
        }

        materials.push_back(mat);
    }
}
