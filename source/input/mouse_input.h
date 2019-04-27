
#ifndef MOUSE_INPUT_H
#define MOUSE_INPUT_H

#include "GLFW/glfw3.h"

namespace MouseInput
{

extern double 
    xScroll, yScroll, 
    // mouse coordinates are NOT in pixels, but screen coordinates.
    mouseX, mouseY, deltaMouseX, deltaMouseY;

void setInputWindow(GLFWwindow* window);

void update();

// if lockedMode is enabled the cursor will be hidden and locked to the center of the screen
void setLockedMode(bool lockedMode);

// 'button' must be 0-7. See https://www.glfw.org/docs/latest/group__buttons.html
// you can also use GLFW_MOUSE_BUTTON_LEFT for example.
bool justPressed(int button);

bool pressed(int button);

bool justReleased(int button);

void setMousePos(double x, double y);

}

#endif
