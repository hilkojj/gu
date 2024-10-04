
#include "json_model_loader.h"

#include "../../../files/file_utils.h"
#include "../../../utils/gu_error.h"

std::string basePath(const std::string &filePath)
{
    for (int i = filePath.size() - 1; i >= 0; i--)
        if (filePath[i] == '/' || filePath[i] == '\\')
            return filePath.substr(0, i + 1);
    return "";
}

std::vector<SharedModel> JsonModelLoader::fromJsonFile(const char *path, const VertAttributes *predefinedAttrs)
{
    JsonModelLoader loader(json::parse(fu::readString(path)), path, predefinedAttrs, basePath(path));
    return loader.models;
}

std::vector<SharedModel> JsonModelLoader::fromUbjsonFile(const char *path, const VertAttributes *predefinedAttrs)
{
    JsonModelLoader loader(json::from_ubjson(fu::readBinary(path)), path, predefinedAttrs, basePath(path));
    return loader.models;
}

JsonModelLoader::JsonModelLoader(const json &obj, std::string id, const VertAttributes *predefinedAttrs, std::string textureBasePath)
: obj(obj), id(id), predefinedAttrs(predefinedAttrs), textureBasePath(textureBasePath)
{
    loadArmatures();
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
            su::split(info["id"], "_part").at(0),
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
    else if (str == "NORMAL")
        return VertAttributes::NORMAL;
    else if (str == "TEXCOORD0")
        return VertAttributes::TEX_COORDS;
    else if (str == "BLENDWEIGHT0")
        return VertAttributes::BONE_WEIGHT_0;
    else if (str == "BLENDWEIGHT1")
        return VertAttributes::BONE_WEIGHT_1;
    else if (str == "BLENDWEIGHT2")
        return VertAttributes::BONE_WEIGHT_2;
    else if (str == "BLENDWEIGHT3")
        return VertAttributes::BONE_WEIGHT_3;
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

//            if (partJson.contains("bones"))
//            {
//                for (const json &boneJson : partJson["bones"])
//                {
//                    for (auto &arm : armatures)
//                    {
//                        auto &bone = arm->bonesByName[boneJson["node"]];
//                        if (bone == NULL) continue;
//                        modelPart.bones.push_back(bone);
//                        modelPart.armature = arm;
//                    }
//                }
//            }
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
                asset<Texture> tex((textureBasePath + std::string(texJson.at("filename"))).c_str());

                if (texJson.at("type") == "DIFFUSE")
                    mat->diffuseTexture.assetTex = tex;
                else if (texJson.at("type") == "SPECULAR")
                    mat->specularMap.assetTex = tex;
                else if (texJson.at("type") == "BUMP")
                    mat->normalMap.assetTex = tex;
            }
        }

        materials.push_back(mat);
    }
}

void readTransScaleAndRot(const json &j, vec3 &trans, vec3 &scale, quat &rot)
{
    if (j.contains("translation"))
        trans = vec3(j["translation"][0], j["translation"][1], j["translation"][2]);
    if (j.contains("scale"))
        scale = vec3(j["scale"][0], j["scale"][1], j["scale"][2]);
    if (j.contains("rotation"))
        rot = quat(j["rotation"][3], j["rotation"][0], j["rotation"][1], j["rotation"][2]);
}

void loadChildBones(const json &boneJson, std::vector<SharedBone> &out, SharedArmature &arm)
{
    if (!boneJson.contains("children")) return;

    for (const json &childJson : boneJson["children"])
    {
        auto &child = out.emplace_back(new Bone);
        child->name = childJson.at("id");
        arm->bones.push_back(child);
//        arm->bonesByName[child->name] = child;

        readTransScaleAndRot(childJson, child->translation, child->scale, child->rotation);

        loadChildBones(childJson, child->children, arm);
    }
}

void JsonModelLoader::loadArmatures()
{
    if (!obj.contains("nodes")) return;

    for (const json &armJson : obj["nodes"])
    {
        if (armJson.contains("parts")) continue;  // assume this is a model.

        SharedArmature arm(new Armature { armJson["id"] });

        std::vector<SharedBone> rootBones;
        loadChildBones(armJson, rootBones, arm);

        if (rootBones.size() > 1) throw gu_err(arm->name + " has more than 1 root bones!");
        else if (rootBones.empty()) continue;

        arm->root = rootBones[0];
        if (armJson.contains("translation"))
            arm->root->translation += vec3(armJson["translation"][0], armJson["translation"][1], armJson["translation"][2]);

        armatures.push_back(arm);
    }

    if (!obj.contains("animations")) return;

#if 0
    for (const json &animJson : obj["animations"])
    {
        if (!animJson.contains("bones")) continue;  // this is not an armature animation.

        for (const json &boneJson : animJson["bones"])
        {
            if (!boneJson.contains("keyframes")) continue;

            for (auto &arm : armatures)
            {
                auto &bone = arm->bonesByName[boneJson["boneId"]];
                if (bone == NULL) continue;

                for (const json &keyJson : boneJson["keyframes"])
                {
                    auto &anim = arm->animations[animJson.at("id")];
                    auto &keyFrame = anim.keyFramesPerBone[bone].emplace_back();
                    keyFrame.keyTime = keyJson.at("keytime");
                    keyFrame.keyTime /= 1000.f;
                    anim.duration = max(keyFrame.keyTime, anim.duration);
                    readTransScaleAndRot(keyJson, keyFrame.translation, keyFrame.scale, keyFrame.rotation);

                    if (keyJson.contains("translation"))
                        keyFrame.translation = vec3(keyJson["translation"][0], keyJson["translation"][1], keyJson["translation"][2]);
                    else
                        keyFrame.translation = bone->translation;
                    if (keyJson.contains("scale"))
                        keyFrame.scale = vec3(keyJson["scale"][0], keyJson["scale"][1], keyJson["scale"][2]);
                    else
                        keyFrame.scale = bone->scale;
                    if (keyJson.contains("rotation"))
                        keyFrame.rotation = quat(keyJson["rotation"][3], keyJson["rotation"][0], keyJson["rotation"][1], keyJson["rotation"][2]);
                    else
                        keyFrame.rotation = bone->rotation;
                }
            }
        }
    }
#endif
}
