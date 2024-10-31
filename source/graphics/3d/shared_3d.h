#pragma once

#include <memory>

class Model;
typedef std::shared_ptr<Model> SharedModel;

class Mesh;
typedef std::shared_ptr<Mesh> SharedMesh;

struct Material;
typedef std::shared_ptr<Material> SharedMaterial;

struct Armature;
typedef std::shared_ptr<Armature> SharedArmature;

struct Bone;
typedef std::shared_ptr<Bone> SharedBone;
