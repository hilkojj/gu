#include <iostream>

#include "gu/game_utils.h"

#include "test_screens/triangle_screen.cpp"
#include "test_screens/mesh_screen.cpp"
#include "test_screens/instanced_mesh_screen.cpp"
#include "test_screens/web_assembly_screen.cpp"

int main()
{
    gu::Config config;
    #ifdef EMSCRIPTEN
    config.width = 900;
    config.height = 506;
    #endif

    if (!gu::init(config))
        return -1;

    std::cout << glGetString(GL_VERSION) << std::endl;

    MeshScreen scr;
    gu::setScreen(&scr);

    gu::run();
    
    return 0;
}
