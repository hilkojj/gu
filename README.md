This is a C++ game library for creating games with OpenGL for Desktop and Web.

## Web demo

A *very* simple online demo using the game library can be seen [here](https://hilkojj.github.io/cpp-game-utils/emscripten_test/gutest.html).

Controls: WASD + mouse (click on the screen to lock cursor)


A more complex webdemo of my own game made using this library can be seen [here](https://hilkojj.github.io/).

## Usage

Clone/fork [this template](https://github.com/hilkojj/cpp-game-utils-template) and follow the instructions in the readme to create a minimal project that uses this library.

## Features

- Graphics:
    - Textures (loading `.DDS` files, TextureArrays, CubeMap etc.)
    - FrameBuffers
    - ShaderPrograms
    - Meshes, Models & ModelInstances
    - Cameras
- Audio:
    - Loading `.wav` files
    - Playing sounds using OpenAL
- AssetManager
    - Live reload of assets (linux only)
- Mouse & key input
- Loading models from Blender (use [this Blender addon](https://github.com/hilkojj/blender_UBJSON_exporter) and load the exported files with the JsonModelLoader)
- Loading [Aseprite](https://www.aseprite.org/) sprites directly from [`.ase` files](https://github.com/aseprite/aseprite/blob/master/docs/ase-file-specs.md). (Only indexed sprites supported) 
- Some utilities (Math, TangentCalculator, A* pathfinder, etc.)

## Manual installation
(Not recommended, use the template above instead)

1. Download GLFW (version 3.2.1) from [here](https://www.glfw.org/) and extract it to `./external/glfw-3.2.1`

2. Generate & download the OpenGL loader with [Glad](https://glad.dav1d.de/) (select gl Version 4.3 and add extention 'GL_EXT_texture_compression_s3tc') and extract the .zip to `./external/glad`

3. Download [GLM (OpenGL Mathematics for C++) version 0.9.9.5](https://github.com/g-truc/glm/releases) and extract to `./external/glm-0.9.9.5`

4. Download `json.hpp` from [JSON for Modern C++ (version 3.6.1)](https://github.com/nlohmann/json/releases/tag/v3.6.1) and place it in `./external`

5. Add the library to a CMake project:
   
    `add_subdirectory(cpp-game-utils/library/)`

    `target_link_libraries(myprojectname gameutils)`
