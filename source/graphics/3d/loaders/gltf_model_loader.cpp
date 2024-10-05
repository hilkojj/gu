
#include "gltf_model_loader.h"

#include "../mesh.h"
#include "../model.h"
#include "../armature.h"
#include "../tangent_calculator.h"

#include "../../textures/texture.h"
#include "../../external/stb_image.h"

#include "../../../utils/gu_error.h"

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include "../../../../external/tiny_gltf.h"

GltfModelLoader::GltfModelLoader(const VertAttributes &attrs) :
    vertAttributes(attrs)
{}

int componentTypeSize(int componentType)
{
    switch (componentType)
    {
        case GL_FLOAT:
            assert(sizeof(int32) == sizeof(float));
        case GL_INT:
        case GL_UNSIGNED_INT:
            return sizeof(int32);
        case GL_SHORT:
        case GL_UNSIGNED_SHORT:
            return sizeof(int16);
        case GL_BYTE:
        case GL_UNSIGNED_BYTE:
            return sizeof(int8);
    }
    throw gu_err("Error while loading glTF: " + std::to_string(componentType) + " not recognized as a component type (GL_FLOAT, GL_BYTE, etc..)");
}

int nrOfVertices(const tinygltf::Model &tiny, const tinygltf::Mesh &tinyMesh)
{
    int nrOfVerts = 0;
    for (auto &primitive : tinyMesh.primitives)
    {
        for (auto &[attrName, accessorI] : primitive.attributes)
        {
            nrOfVerts += tiny.accessors.at(accessorI).count;
            break;
        }
    }
    return nrOfVerts;
}

void loadMeshes(GltfModelLoader &loader, const tinygltf::Model &tiny)
{
    for (auto &tinyMesh : tiny.meshes)
    {
        int nrOfVerts = nrOfVertices(tiny, tinyMesh);
        loader.meshes.push_back(std::make_shared<Mesh>(tinyMesh.name, nrOfVerts, loader.vertAttributes));
        auto &mesh = loader.meshes.back();

        int nrOfVertsLoaded = 0;

        for (auto &primitive : tinyMesh.primitives)
        {
            if (primitive.indices < 0)
                continue;

            auto &part = mesh->parts.emplace_back();
            part.mode = primitive.mode;

            // indices:
            {
                auto &accessor = tiny.accessors.at(primitive.indices);

                if (accessor.componentType != GL_UNSIGNED_SHORT)
                    throw gu_err("Error while loading glTF: only unsigned shorts are supported as indices type.");

                if (accessor.count > 0)
                {
                    part.indices.resize(accessor.count);

                    auto &bufferView = tiny.bufferViews.at(accessor.bufferView);
                    auto &buffer = tiny.buffers.at(bufferView.buffer);

                    if (buffer.data.size() < bufferView.byteOffset + bufferView.byteLength)
                        throw gu_err("Error while loading glTF: buffer is too small");

                    if (bufferView.byteLength != part.indices.size() * sizeof(unsigned short))
                        throw gu_err("Error while loading glTF: indices bufferView byteLength is incorrect");

                    memcpy(&part.indices[0], &buffer.data.at(bufferView.byteOffset), bufferView.byteLength);
                    for (auto &index : part.indices)
                        index += nrOfVertsLoaded;
                }
            }

            // vertices:
            bool tangentsProvided = false;
            int primitiveVerts = 0;
            for (auto &[attrName, accessorI] : primitive.attributes)
            {
                auto &accessor = tiny.accessors.at(accessorI);
                primitiveVerts = accessor.count;

                VertAttr attr { attrName };
                if (attrName == "TEXCOORD_0")
                    attr = VertAttributes::TEX_COORDS;
                else if (attrName == "COLOR_0")
                    attr = accessor.type == 3 ? VertAttributes::RGB : VertAttributes::RGBA;
                else if (attrName == "TANGENT" && accessor.type == TINYGLTF_TYPE_VEC4)
                {
                    attr = VertAttributes::TANGENT_AND_SIGN;
                    tangentsProvided = true;
                }

                attr.type = accessor.componentType;
                attr.size = accessor.type;
                attr.byteSize = componentTypeSize(attr.type) * attr.size;
                attr.normalized = accessor.normalized;

                if (!loader.vertAttributes.contains(attr))
                    continue;

                auto &bufferView = tiny.bufferViews.at(accessor.bufferView);
                auto &buffer = tiny.buffers.at(bufferView.buffer);

                if (buffer.data.size() < bufferView.byteOffset + bufferView.byteLength)
                    throw gu_err("Error while loading glTF: buffer is too small");

                if (bufferView.byteLength != primitiveVerts * attr.byteSize)
                    throw gu_err("Error while loading glTF: vertices bufferView byteLength is incorrect");

                auto attrOffset = loader.vertAttributes.getOffset(attr);

                int tinyVertI = 0;
                for (int vertI = nrOfVertsLoaded; vertI < nrOfVertsLoaded + primitiveVerts; vertI++)
                {
                    for (int component = 0; component < attr.size; component++)
                    {
                        switch (attr.type)
                        {
                            case GL_FLOAT:
                                assert(sizeof(int32) == sizeof(float));
                            case GL_INT:
                            case GL_UNSIGNED_INT:
                                mesh->get<float>(vertI, attrOffset + component * sizeof(float)) = *((float *) &buffer.data.at(bufferView.byteOffset + tinyVertI * attr.byteSize + component * sizeof(float)));
                                break;
                            case GL_SHORT:
                            case GL_UNSIGNED_SHORT:
                                mesh->get<short>(vertI, attrOffset + component * sizeof(short)) = *((short *) &buffer.data.at(bufferView.byteOffset + tinyVertI * attr.byteSize + component * sizeof(short)));
                                break;
                            case GL_BYTE:
                            case GL_UNSIGNED_BYTE:
                                mesh->get<char>(vertI, attrOffset + component * sizeof(char)) = *((char *) &buffer.data.at(bufferView.byteOffset + tinyVertI * attr.byteSize + component * sizeof(char)));
                                break;
                        }
                    }
                    tinyVertI++;
                }
            }
            nrOfVertsLoaded += primitiveVerts;

            switch (loader.calculateTangents)
            {
                case GltfModelLoader::NEVER:
                    break;
                case GltfModelLoader::IF_NOT_PROVIDED:
                    if (tangentsProvided)
                    {
                        break;
                    }
                case GltfModelLoader::ALWAYS:
                    TangentCalculator::addTangentsToMesh(mesh, mesh->parts.size() - 1);
                    break;
            }
        }
        assert(nrOfVertsLoaded == nrOfVerts);
    }
}

void loadModels(GltfModelLoader &loader, const tinygltf::Model &tiny)
{
    for (auto &node : tiny.nodes)
    {
        if (node.mesh < 0)
            continue;

        loader.models.push_back(std::make_shared<Model>(node.name));
        auto &model = loader.models.back();
        auto &mesh = loader.meshes.at(node.mesh);
        auto &tinyMesh = tiny.meshes.at(node.mesh);
        int partI = 0;
        for (auto &meshPart : mesh->parts)
        {
            auto &modelPart = model->parts.emplace_back();
            modelPart.mesh = mesh;

            auto materialI = tinyMesh.primitives.at(partI).material;
            if (materialI >= 0)
                modelPart.material = loader.materials.at(materialI);

            if (node.skin >= 0)
                modelPart.armature = loader.armatures.at(node.skin);

            modelPart.meshPartIndex = partI++;
        }
    }
}

template<int length=3>
void stdVectorToGlmVec(const std::vector<double> &stdVector, vec<length, float, defaultp> &glmVec)
{
    if (stdVector.size() < length)
        throw gu_err("stdVector.size() < length");
    for (int i = 0; i < length; i++)
        glmVec[i] = stdVector.at(i);
}

template<class TinyTextureInfo>
void loadImageData(const GltfModelLoader &loader, const TinyTextureInfo &info, SharedTexture &out)
{
    if (info.index < 0)
        return;
    out = loader.textures.at(info.index);
}

void loadMaterials(GltfModelLoader &loader, const tinygltf::Model &tiny)
{
    for (auto &tinyMaterial : tiny.materials)
    {
        loader.materials.push_back(std::make_shared<Material>());
        auto &material = loader.materials.back();
        material->name = tinyMaterial.name;
        material->doubleSided = tinyMaterial.doubleSided;
        material->roughness = tinyMaterial.pbrMetallicRoughness.roughnessFactor;
        material->metallic = tinyMaterial.pbrMetallicRoughness.metallicFactor;
        material->specular = vec4(1.0f);
        for (auto &[key, value] : tinyMaterial.extensions)
        {
            if (key == "KHR_materials_specular")
            {
                if (!value.IsObject())
                {
                    throw gu_err("Could not read specular");
                }
                bool bReadSpecular = false;
                const tinygltf::Value::Object &specObj = value.Get<tinygltf::Value::Object>();

                auto colorFactorIt = specObj.find("specularColorFactor");
                if (colorFactorIt != specObj.end() && colorFactorIt->second.IsArray())
                {
                    const tinygltf::Value &specArray = colorFactorIt->second;
                    switch (specArray.Size())
                    {
                        case 1:
                            material->specular = vec4(float(specArray.Get(0).Get<double>()));
                            break;
                        case 3:
                            material->specular = vec4(specArray.Get(0).Get<double>(), specArray.Get(1).Get<double>(),
                                specArray.Get(2).Get<double>(), 1.0f);
                        case 4:
                            material->specular = vec4(specArray.Get(0).Get<double>(), specArray.Get(1).Get<double>(),
                                specArray.Get(2).Get<double>(), specArray.Get(3).Get<double>());
                            break;
                        default:
                            break;
                    }
                    bReadSpecular = true;
                }

                auto valueFactorIt = specObj.find("specularFactor");
                if (valueFactorIt != specObj.end() && valueFactorIt->second.IsNumber())
                {
                    const float valueFactor = valueFactorIt->second.GetNumberAsDouble();
                    material->specular *= valueFactor;
                    bReadSpecular = true;
                }

                if (!bReadSpecular)
                {
                    throw gu_err("Specular extension found, but no `specularColorFactor` or `specularFactor` found!");
                }
            }
        }

        stdVectorToGlmVec(tinyMaterial.pbrMetallicRoughness.baseColorFactor, material->diffuse);
        stdVectorToGlmVec(tinyMaterial.emissiveFactor, material->emissive);

        if (loader.loadDiffuseTextures)
            loadImageData(loader, tinyMaterial.pbrMetallicRoughness.baseColorTexture, material->diffuseTexture);
        if (loader.loadNormalMaps)
            loadImageData(loader, tinyMaterial.normalTexture, material->normalMap);
        material->normalMapScale = tinyMaterial.normalTexture.scale;
        if (loader.loadMetallicRoughnessTextures)
            loadImageData(loader, tinyMaterial.pbrMetallicRoughness.metallicRoughnessTexture, material->metallicRoughnessTexture);
        if (loader.loadEmissiveTextures)
            loadImageData(loader, tinyMaterial.emissiveTexture, material->emissiveTexture);
        if (loader.loadAOTextures)
            loadImageData(loader, tinyMaterial.occlusionTexture, material->aoTexture);
        material->aoTextureStrength = tinyMaterial.occlusionTexture.strength;
    }
}

void loadTextures(GltfModelLoader &loader, const tinygltf::Model &tiny)
{
    for (auto &tinyTexture : tiny.textures)
    {
        if (tinyTexture.source >= 0)
        {
            auto &tinyImage = tiny.images.at(tinyTexture.source);

            if (tinyImage.component <= 0 || tinyImage.component > 4)
                throw gu_err(tinyImage.name + " has " + std::to_string(tinyImage.component) + " channels!");

            if (tinyImage.pixel_type != GL_UNSIGNED_BYTE)
                throw gu_err("pixel data of " + tinyImage.name + " is not GL_UNSIGNED_BYTE!");

            GLenum format = std::vector<GLenum>{GL_R, GL_RG, GL_RGB, GL_RGBA}[tinyImage.component - 1];

            loader.textures.push_back(SharedTexture(new Texture(Texture::fromByteData(
                tinyImage.image.data(), format, format, tinyImage.width, tinyImage.height,
                loader.textureMagFilter, loader.textureMinFilter, loader.generateMipMaps
            ))));
        }
        else loader.textures.push_back(SharedTexture());
    }
}

void loadArmatures(GltfModelLoader &loader, const tinygltf::Model &tiny)
{
    std::unordered_map<int, SharedBone> bones;
    std::unordered_map<int, SharedArmature> boneToArmature;
    std::unordered_map<int, Armature::Animation> animations;
    std::unordered_map<int, Armature::Animation::SharedTimeline> timelines;
    std::unordered_map<int, Armature::Animation::SharedPropertyValues> propertyValues;

    for (auto &tinySkin : tiny.skins)
    {
        auto armature = std::make_shared<Armature>();
        armature->name = tinySkin.name;

        for (auto &jointNodeIndex : tinySkin.joints)
        {
            auto jointNode = tiny.nodes[jointNodeIndex];
            auto bone = std::make_shared<Bone>();
            bone->name = jointNode.name;
            if (!jointNode.rotation.empty())
                bone->rotation = quat(jointNode.rotation[3], jointNode.rotation[0], jointNode.rotation[1], jointNode.rotation[2]);
            if (!jointNode.translation.empty())
                stdVectorToGlmVec(jointNode.translation, bone->translation);
            if (!jointNode.scale.empty())
                stdVectorToGlmVec(jointNode.scale, bone->scale);

            bones[jointNodeIndex] = bone;
            boneToArmature[jointNodeIndex] = armature;
            armature->bones.push_back(bone);
        }

        for (auto &jointNodeIndex : tinySkin.joints)
        {
            auto jointNode = tiny.nodes[jointNodeIndex];
            auto bone = bones[jointNodeIndex];
            for (auto &childI : jointNode.children)
            {
                if (bones.find(childI) == bones.end())
                {
                    throw gu_err("Bone not loaded.");
                }
                bone->children.push_back(bones[childI]);
                bones[childI]->parent = bone;
            }
        }

        if (bones.find(tinySkin.skeleton) != bones.end())
        {
            armature->root = bones[tinySkin.skeleton];
        }

        // inverse bind matrices:
        if (tinySkin.inverseBindMatrices >= 0)
        {
            auto &ibmAccessor = tiny.accessors.at(tinySkin.inverseBindMatrices);

            auto &bufferView = tiny.bufferViews.at(ibmAccessor.bufferView);
            auto &buffer = tiny.buffers.at(bufferView.buffer);

            assert(ibmAccessor.componentType == GL_FLOAT);
            assert(ibmAccessor.type == TINYGLTF_TYPE_MAT4);
            assert(bufferView.byteLength == ibmAccessor.count * sizeof(mat4));
            assert(buffer.data.size() >= (bufferView.byteOffset + bufferView.byteLength));
            for (int i = 0; i < ibmAccessor.count; i++)
            {
                armature->bones.at(i)->inverseBindMatrix = *((mat4 *) &buffer.data[bufferView.byteOffset + i * sizeof(mat4)]);
            }
        }

        loader.armatures.push_back(armature);
    }

    int animI = 0;
    for (auto &tinyAnim : tiny.animations)
    {
        auto &anim = animations[animI++];
        anim.name = tinyAnim.name;

        for (auto &tinySampler : tinyAnim.samplers)
        {
            auto existingTimelineIt = timelines.find(tinySampler.input);
            if (existingTimelineIt == timelines.end())
            {
                // load timeline.
                auto &timeline = timelines[tinySampler.input] = std::make_shared<Armature::Animation::Timeline>();

                auto &accessor = tiny.accessors.at(tinySampler.input);
                timeline->times.resize(accessor.count, 0);
                if (accessor.componentType != GL_FLOAT)
                {
                    throw gu_err("Timeline does not contain floats.");
                }
                auto &bufferView = tiny.bufferViews.at(accessor.bufferView);
                auto &buffer = tiny.buffers.at(bufferView.buffer);
                assert(bufferView.byteLength == accessor.count * sizeof(float));
                assert(buffer.data.size() >= (bufferView.byteOffset + bufferView.byteLength));
                for (int i = 0; i < accessor.count; i++)
                {
                    timeline->times[i] = *((float *) &buffer.data[bufferView.byteOffset + i * sizeof(float)]);
                    anim.duration = max(timeline->times[i], anim.duration);
                }
            }
            else
            {
                for (const float time : existingTimelineIt->second->times)
                {
                    anim.duration = max(time, anim.duration);
                }
            }

            if (propertyValues.find(tinySampler.output) == propertyValues.end())
            {
                // load propertyValues.
                auto &values = propertyValues[tinySampler.output] = std::make_shared<Armature::Animation::PropertyValues>();

                auto &accessor = tiny.accessors.at(tinySampler.output);
                if (accessor.componentType != GL_FLOAT)
                {
                    throw gu_err("Found an animation (" + tinyAnim.name + ") with componentType " + std::to_string(accessor.componentType));
                }
                auto &bufferView = tiny.bufferViews.at(accessor.bufferView);
                auto &buffer = tiny.buffers.at(bufferView.buffer);
                assert(buffer.data.size() >= (bufferView.byteOffset + bufferView.byteLength));
                if (accessor.type == TINYGLTF_TYPE_VEC4)
                {
                    // quats
                    values->quatValues.resize(accessor.count);
                    assert(bufferView.byteLength == accessor.count * sizeof(vec4));
                    for (int i = 0; i < accessor.count; i++)
                    {
                        vec4 xyzw = *((vec4 *) &buffer.data[bufferView.byteOffset + i * sizeof(vec4)]);
                        values->quatValues[i] = quat(xyzw[3], xyzw[0], xyzw[1], xyzw[2]);
                    }
                }
                else if (accessor.type == TINYGLTF_TYPE_VEC3)
                {
                    // vec3s
                    values->vec3Values.resize(accessor.count);
                    assert(bufferView.byteLength == accessor.count * sizeof(vec3));
                    for (int i = 0; i < accessor.count; i++)
                    {
                        values->vec3Values[i] = *((vec3 *) &buffer.data[bufferView.byteOffset + i * sizeof(vec3)]);
                    }
                }
                else std::cerr << "Found an animation (" << tinyAnim.name << ") with property values with type " << accessor.type << ". Animation will not work!" << std::endl;
            }

        }

        for (auto &tinyChannel : tinyAnim.channels)
        {
            if (bones.find(tinyChannel.target_node) == bones.end())
                continue;

            auto &channel = anim.channels.emplace_back();
            channel.target = bones[tinyChannel.target_node];

            auto &tinySampler = tinyAnim.samplers[tinyChannel.sampler];

            if (tinySampler.interpolation == "STEP")
                channel.interpolation = Armature::Animation::Channel::STEP;
            else if (tinySampler.interpolation == "LINEAR")
                channel.interpolation = Armature::Animation::Channel::LINEAR;
            else if (tinySampler.interpolation == "CUBICSPLINE")
                channel.interpolation = Armature::Animation::Channel::CUBICSPLINE;
            else throw gu_err("Channel has unrecognized interpolation: " + tinySampler.interpolation);

            channel.timeline = timelines.at(tinySampler.input);
            channel.propertyValues = propertyValues.at(tinySampler.output);

            if (tinyChannel.target_path == "translation")
            {
                channel.targetProperty = Armature::Animation::Channel::TRANSLATION;
                if (channel.propertyValues->vec3Values.empty())
                    throw gu_err("The translation channel for bone \"" + channel.target->name + "\" for animation \"" + anim.name + "\" does not have vec3 values!");
            }
            else if (tinyChannel.target_path == "rotation")
            {
                channel.targetProperty = Armature::Animation::Channel::ROTATION;
                if (channel.propertyValues->quatValues.empty())
                    throw gu_err("The rotation channel for bone \"" + channel.target->name + "\" for animation \"" + anim.name + "\" does not have quaternion values!");
            }
            else if (tinyChannel.target_path == "scale")
            {
                channel.targetProperty = Armature::Animation::Channel::SCALE;
                if (channel.propertyValues->vec3Values.empty())
                    throw gu_err("The scale channel for bone \"" + channel.target->name + "\" for animation \"" + anim.name + "\" does not have vec3 values!");
            }
            else throw gu_err("Found a channel for bone \"" + channel.target->name + "\" for animation \"" + anim.name + "\" with target.path == \"" + tinyChannel.target_path + "\"");

            boneToArmature[tinyChannel.target_node]->animations[anim.name] = anim;
        }
    }
}

void load(GltfModelLoader &loader, tinygltf::Model &tiny)
{
    loadTextures(loader, tiny);
    loadMaterials(loader, tiny);
    loadMeshes(loader, tiny);
    loadArmatures(loader, tiny);
    loadModels(loader, tiny);
}

void GltfModelLoader::fromASCIIFile(const char *path)
{
    tinygltf::Model model;
    tinygltf::TinyGLTF ctx;
    std::string err, warn;
    bool success = ctx.LoadASCIIFromFile(&model, &err, &warn, path);

    if (!err.empty())
        throw gu_err(err);
    if (!warn.empty())
        std::cerr << "Warning while loading " << path << ":\n" << warn << std::endl;
    if (!success)
        throw gu_err("Failed to parse glTF");

    load(*this, model);
}

void GltfModelLoader::fromBinaryFile(const char *path)
{
    tinygltf::Model model;
    tinygltf::TinyGLTF ctx;
    std::string err, warn;
    bool success = ctx.LoadBinaryFromFile(&model, &err, &warn, path);

    if (!err.empty())
        throw gu_err(err);
    if (!warn.empty())
        std::cerr << "Warning while loading " << path << ":\n" << warn << std::endl;
    if (!success)
        throw gu_err("Failed to parse glTF");

    load(*this, model);
}
