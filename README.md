## Installation

1. Download GLFW (version 3.2.1) from [here](https://www.glfw.org/) and extract it to `./external/glfw-3.2.1`

2. Generate & download the OpenGL loader with [Glad](https://glad.dav1d.de/) (select gl Version 4.3) and extract the .zip to `./external/glad`

3. Download [GLM (OpenGL Mathematics for C++) version 0.9.9.5](https://github.com/g-truc/glm/releases) and extract to `./external/glm-0.9.9.5`

4. Download `json.hpp` from [JSON for Modern C++ (version 3.6.1)](https://github.com/nlohmann/json/releases/tag/v3.6.1) and place it in `./external`

5. Add the library to a CMake project:
   
    `add_subdirectory(cpp-game-utils/)`

    `target_link_libraries(myprojectname gameutils)`