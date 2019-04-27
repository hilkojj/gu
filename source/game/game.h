#ifndef GAME_H
#define GAME_H

#include "screen.h"
#include "GLFW/glfw3.h"

namespace Game
{

extern GLFWwindow *window;
extern int width, height, widthPixels, heightPixels;

void setScreen(Screen *screen);
void render(double deltaTime);
void onResize();

}; // namespace Game

#endif
