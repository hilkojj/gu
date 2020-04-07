
#include <imgui.h>
#include <examples/imgui_impl_glfw.h>
#include "mouse_input.h"
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

ButtonStatus buttonStatuses[NR_OF_BUTTONS];

void glfwButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
    if (ImGui::GetIO().WantCaptureMouse)
        return;
    buttonStatuses[button] = action == GLFW_PRESS ? JUST_PRESSED : JUST_RELEASED;
}

double nextXScroll = 0, nextYScroll = 0, nextMouseX = 0, nextMouseY = 0;

void glfwScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    #ifdef EMSCRIPTEN
    xoffset *= -.012;
    yoffset *= -.012;
    #endif
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
    if (ImGui::GetIO().WantCaptureMouse)
        return;
    nextXScroll += xoffset;
    nextYScroll += yoffset;
}

void glfwMousePosCallback(GLFWwindow* window, double xpos, double ypos)
{
    nextMouseX = xpos;
    nextMouseY = ypos;
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
    nextMouseX = mouseX;
    nextMouseY = mouseY;
}

void update()
{
    for (int i = 0; i < NR_OF_BUTTONS; i++)
    {
        ButtonStatus& s = buttonStatuses[i];
        if (s == JUST_PRESSED)
            s = PRESSED;
        else if (s == JUST_RELEASED)
            s = NOT_PRESSED;
    }
    xScroll = nextXScroll;
    yScroll = nextYScroll;
    nextXScroll = nextYScroll = 0;
    
    deltaMouseX = nextMouseX - mouseX;
    deltaMouseY = nextMouseY - mouseY;
    mouseX = nextMouseX;
    mouseY = nextMouseY;
    // dont set nextMouse... to 0 here.
}

void setLockedMode(bool lockedMode)
{
    glfwSetInputMode(window, GLFW_CURSOR, lockedMode ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void setMousePos(double x, double y)
{
    glfwSetCursorPos(window, x, y);
}

bool justPressed(int button)
{
    return buttonStatuses[button] == JUST_PRESSED;
}

bool pressed(int button)
{
    return buttonStatuses[button] == PRESSED || buttonStatuses[button] == PRESSED;
}

bool justReleased(int button)
{
    return buttonStatuses[button] == JUST_RELEASED;
}

} // namespace MouseInput
