# GU:
## Game Utilities for use with OpenGL, GLM, GLFW and Dear ImGui

This library forms the lowest layer of my C++ game projects.
The goal is to offer commonly used utilities without dictating the architecture of the game, except for the use of the dependencies.

*Note: A lot of the code was written when I was a C++ noob, so some files still could use some refactoring.*

[Dibidab](https://github.com/dibidabidab/dibidab-engine) is the ECS-based game engine that uses this utility library.

### Dependencies
Most dependencies are included using submodules in `external/`.
Run `git submodule update --init --recursive` before configuring CMake.

Other dependencies are downloaded by CMake as can be seen in `library/CMakeLists.txt`.

This repository includes a stripped down version of the [Bullet physics engine](http://bulletphysics.org/) v2 (4MB).
Including it as a submodule would take >600MB.
Read `external/bullet2/readme.txt` for details on modifications.

### Options
Check `library/CMakeLists.txt` for all options.
The Bullet physics engine is not compiled by default.

### Features

Look at the folder structure in `source/` to get an idea of what is included in this library.

An incomplete list:

- **Wrappers for OpenGL functionality**:
    - Textures
      - TextureArrays (3d textures)
      - CubeMaps
      - Loading images from common formats
      - Loading compressed images from `.DDS`, which are decompressed by the GPU itself
    - FrameBuffers
    - ShaderPrograms
    - Cameras
- **3D**:
  - Meshes
  - Models
  - Animated Armatures
  - `glTF` loader, tested in use with Blender.
  - ~~Json Model loader~~ (can be included using the CMake options), which uses a format originally used by LibGDX and exported through this [Blender addon](https://github.com/dibidabidab/blender_UBJSON_exporter).
- **Asset management**:
  - File assets can be loaded using custom loaders.
  - Assets are available for use everywhere using the `asset<*your-asset-type*>` type.
  - Assets can be live-reloaded on Linux using file change detection.
- **Aseprite**:
  - Structs for [Aseprite](https://www.aseprite.org/) files
  - A loader that loads [`.ase` files](https://github.com/aseprite/aseprite/blob/master/docs/ase-file-specs.md) into these structs
  - A texture generator that uploads the sprites to OpenGL
- **Player input**:
  - Gamepad
  - Keyboard
  - Mouse
- ~~**Audio**~~:
  - There used to be integration with OpenAL (see commit history). The OpenAL implementation that was linked used an annoying license. In my current game project I'm using FMOD instead.
  - Loading `.wav` and `.ogg` files.
- More utilities for math, pathfinding, file reading, etc.
