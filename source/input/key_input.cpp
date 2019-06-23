
#include <map>
#include "key_input.h"
#include "imgui.h"
#include "examples/imgui_impl_glfw.h"

namespace KeyInput
{

namespace
{

enum KeyStatus
{
    NOT_PRESSED,
    JUST_PRESSED,
    PRESSED,
    JUST_RELEASED
};

std::map<int, KeyStatus> keyStatuses;

void glfwCallbackKey(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) // glfwCallbackCharacter() does not get called on Chrome when spacebar is pressed, so do it manually
        ImGui::GetIO().AddInputCharacter(' ');

    if (action == GLFW_REPEAT)
        return;

    // printf("%s was %s\n", glfwGetKeyName(key, 0), action == GLFW_PRESS ? "pressed" : "released");

    keyStatuses[key] = action == GLFW_PRESS ? JUST_PRESSED : JUST_RELEASED;
}

void glfwCallbackCharacter(GLFWwindow *window, unsigned int c)
{
    if (c == ' ') return; // spacebar is handled in glfwCallbackKey()
    ImGui_ImplGlfw_CharCallback(window, c);
}

} // namespace

void setInputWindow(GLFWwindow* window)
{
    glfwSetKeyCallback(window, glfwCallbackKey);
    glfwSetCharCallback(window, glfwCallbackCharacter);
}

void update()
{
    for (auto &[key, status] : keyStatuses)
    {
        if (status == JUST_PRESSED) status = PRESSED;
        else if (status == JUST_RELEASED) status = NOT_PRESSED;
    }
}

bool justPressed(int key)
{
    return keyStatuses[key] == JUST_PRESSED;
}

bool pressed(int key)
{
    KeyStatus& status = keyStatuses[key];
    return status == PRESSED || status == JUST_PRESSED;
}

bool justReleased(int key)
{
    return keyStatuses[key] == JUST_RELEASED;
}

} // namespace KeyInput
