#pragma once

class GLFWwindow;

namespace MouseInput
{

extern double xScroll, yScroll;
extern double
    // mouse coordinates are in pixels.
    mouseX, mouseY, deltaMouseX, deltaMouseY;

void setInputWindow(GLFWwindow *window);

void update();

// if lockedMode is enabled the cursor will be hidden and locked to the center of the screen
void setLockedMode(bool lockedMode);

// 'button' must be 0-7. See https://www.glfw.org/docs/latest/group__buttons.html
// you can also use GLFW_MOUSE_BUTTON_LEFT for example.
bool justPressed(int button, int priority = 0);

bool pressed(int button, int priority = 0);

bool justReleased(int button, int priority = 0);

bool justReleasedNoDrag(int button, int dragPixelsThreshold = 4, int priority = 0);

bool isDragging(int button, int dragPixelsThreshold = 4);

void capture(int button, int priority, int frames = 1);

void setMousePos(double x, double y);

}
