
#include <iostream>
#include "game.h"

namespace Game
{
namespace
{
Screen *screen;
}

void setScreen(Screen *newScreen)
{
    screen = newScreen;
}

void render(double deltaTime)
{
    if (screen)
        screen->render(deltaTime);
}

void end()
{
}

} // namespace Game
