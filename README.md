This is a C++ game library with basic utilities for creating games.
It also includes the basic functionality of the Bullet physics engine (4MB), because adding it as a submodule would take >600MB.

This library is used in my game engine ([repo](https://github.com/dibidabidab/dibidab-engine) of the bare engine without any renderer) ([repo](https://github.com/hilkojj/flipkaas) of the engine with a 3D PBR renderer, and more 3D stuff).

## Features
(Incomplete list because I update this list like 2 times a decade.)

- Graphics using OpenGL:
    - Textures (loading `.DDS`/`.PNG`/etc. files, TextureArrays, CubeMap etc.)
    - FrameBuffers
    - ShaderPrograms
    - Meshes, Models & animated Armatures
    - Cameras
- Audio:
    - Loading `.wav` and `.ogg` files
    - ~~Playing sounds using OpenAL~~ To be replaced with something else.
- AssetManager
    - Live reload of assets (automatically using file change detection on Linux)
- Mouse & key input
- Loading models from Blender (use the GltfModelLoader to load .glb or .gltf) (~~use [this Blender addon](https://github.com/hilkojj/blender_UBJSON_exporter) and load the exported files with the JsonModelLoader~~)
- Loading [Aseprite](https://www.aseprite.org/) sprites directly from [`.ase` files](https://github.com/aseprite/aseprite/blob/master/docs/ase-file-specs.md). (Only indexed sprites supported) 
- Some utilities (Math, TangentCalculator, A* pathfinder, etc.)
