
#include "mouse_input.h"

#include "../gu/game_utils.h"
#include "../graphics/external/gl_includes.h"

#include <imgui.h>
#include <examples/imgui_impl_glfw.h>

#include <iostream>
#include <vector>

#include "imgui_internal.h"

#define NR_OF_BUTTONS 8

namespace MouseInput
{

namespace
{

GLFWwindow *window;

enum ButtonStatus
{
    NOT_PRESSED,
    JUST_PRESSED,
    PRESSED,
    JUST_RELEASED
};

bool buttonJustPressed[NR_OF_BUTTONS];
bool buttonPressed[NR_OF_BUTTONS];
bool buttonJustReleased[NR_OF_BUTTONS];

struct Capture
{
    int priority, framesRemaining;
};

std::vector<Capture> captures[NR_OF_BUTTONS];
int highestCapturePriority[NR_OF_BUTTONS];
double pixelsDraggedWhilePressed[NR_OF_BUTTONS];

void glfwButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
    if (ImGui::GetIO().WantCaptureMouse && !(buttonPressed[button] && action == GLFW_RELEASE))
        return;
    if (action == GLFW_PRESS)
    {
        buttonJustPressed[button] = true;
        buttonPressed[button] = true;
    }
    else
    {
        buttonPressed[button] = false;
        buttonJustReleased[button] = true;
    }
}

double nextXScroll = 0, nextYScroll = 0, nextMouseX = 0, nextMouseY = 0;

void glfwScrollCallback(GLFWwindow *window, double x, double y)
{
    ImGui_ImplGlfw_ScrollCallback(window, x, y);
    if (ImGui::GetIO().WantCaptureMouse)
        return;
#ifdef EMSCRIPTEN
    x *= -0.012;
    y *= -0.012;
#endif
    nextXScroll += x;
    nextYScroll += y;
}

void screenCoordsToPixels(double &x, double &y)
{
    if (gu::virtualWidth > 0)
    {
        x /= gu::virtualWidth;
        x *= gu::pixelWidth;
    }
    if (gu::virtualHeight > 0)
    {
        y /= gu::virtualHeight;
        y *= gu::pixelHeight;
    }
}

void pixelsToScreenCoords(double &x, double &y)
{
    if (gu::pixelWidth > 0)
    {
        x /= gu::pixelWidth;
        x *= gu::virtualWidth;
    }
    if (gu::pixelHeight > 0)
    {
        y /= gu::pixelHeight;
        y *= gu::virtualHeight;
    }
}

void glfwMousePosCallback(GLFWwindow *window, double xScreenCoord, double yScreenCoord)
{
    nextMouseX = xScreenCoord;
    nextMouseY = yScreenCoord;
    screenCoordsToPixels(nextMouseX, nextMouseY);
}

} // namespace

double xScroll = 0, yScroll = 0, mouseX = 0, mouseY = 0, deltaMouseX = 0, deltaMouseY = 0;

void setInputWindow(GLFWwindow *inputWindow)
{
    window = inputWindow;

    glfwSetMouseButtonCallback(window, glfwButtonCallback);
    glfwSetScrollCallback(window, glfwScrollCallback);
    glfwSetCursorPosCallback(window, glfwMousePosCallback);

    glfwGetCursorPos(window, &mouseX, &mouseY);
    screenCoordsToPixels(mouseX, mouseY);
    nextMouseX = mouseX;
    nextMouseY = mouseY;
}

void update()
{
    for (int i = 0; i < NR_OF_BUTTONS; i++)
    {
        highestCapturePriority[i] = 0;
        for (int ci = captures[i].size() - 1; ci >= 0; ci--)
        {
            auto &c = captures[i][ci];
            if (c.framesRemaining-- <= 0)
            {
                captures[i][ci] = captures[i].back();
                captures[i].pop_back();
                continue;
            }
            if (c.priority > highestCapturePriority[i])
                highestCapturePriority[i] = c.priority;
        }
    }

    for (int i = 0; i < NR_OF_BUTTONS; i++)
    {
        buttonJustPressed[i] = false;
        buttonJustReleased[i] = false;
    }
    xScroll = nextXScroll;
    yScroll = nextYScroll;
    nextXScroll = nextYScroll = 0;
    
    deltaMouseX = nextMouseX - mouseX;
    deltaMouseY = nextMouseY - mouseY;
    mouseX = nextMouseX;
    mouseY = nextMouseY;
    // dont set nextMouse... to 0 here.

    for (int i = 0; i < NR_OF_BUTTONS; i++)
    {
        if (buttonPressed[i])
        {
            pixelsDraggedWhilePressed[i] += abs(deltaMouseX) + abs(deltaMouseY);
        }
        else
        {
            pixelsDraggedWhilePressed[i] = 0;
        }
    }
}

void setLockedMode(bool lockedMode)
{
    int newVal = lockedMode ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;
    if (newVal != glfwGetInputMode(window, GLFW_CURSOR))
    {
        std::cout << "Locking cursor: " << lockedMode << std::endl;
        glfwSetInputMode(window, GLFW_CURSOR, newVal);
    }
}

void setMousePos(double x, double y)
{
    pixelsToScreenCoords(x, y);
    glfwSetCursorPos(window, x, y);
}

bool justPressed(int button, int priority)
{
    if (buttonJustPressed[button] && highestCapturePriority[button] <= priority)
        return true;
    return false;
}

bool pressed(int button, int priority)
{
    if (buttonPressed[button] && highestCapturePriority[button] <= priority)
        return true;
    return false;
}

bool justReleased(int button, int priority)
{
    if (buttonJustReleased[button] && highestCapturePriority[button] <= priority)
        return true;
    return false;
}

bool justReleasedNoDrag(int button, int dragPixelsThreshold, int priority)
{
    return !isDragging(button, dragPixelsThreshold) && justReleased(button, priority);
}

bool isDragging(int button, int dragPixelsThreshold)
{
    return pixelsDraggedWhilePressed[button] >= dragPixelsThreshold;
}

void capture(int button, int priority, int frames)
{
    Capture c {priority, frames};
    captures[button].push_back(c);
}

} // namespace MouseInput
