
#include <iostream>
#include "game.h"

namespace Game
{

GLFWwindow *window = nullptr;

// size of the window:
// width & height are in screen coordinates, widthPixels & heightPixels are in pixels.
int width = 0, height = 0, widthPixels = 0, heightPixels = 0;

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

void onResize()
{
    if (screen)
        screen->onResize();
}

} // namespace Game
