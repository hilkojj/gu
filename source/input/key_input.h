
#ifndef KEY_INPUT_H
#define KEY_INPUT_H

#include <iostream>

#include "GLFW/glfw3.h"

namespace KeyInput
{

void setInputWindow(GLFWwindow* window);

void update();

bool justPressed(int key);

bool pressed(int key);

bool justReleased(int key);

} // namespace KeyInput

#endif
