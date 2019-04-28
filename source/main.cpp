#include <iostream>

#include "gu/game_utils.h"

#include "test_screens/triangle_screen.cpp"
#include "test_screens/mesh_screen.cpp"

int main()
{
    if (!gu::init({}))
        return -1;

    MeshScreen scr;
    gu::setScreen(&scr);

    gu::run();
    
    return 0;
}
