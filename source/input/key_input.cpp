
#include <map>
#include "key_input.h"

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
} // namespace

void glfwCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_REPEAT)
        return;

    // printf("%s was %s\n", glfwGetKeyName(key, 0), action == GLFW_PRESS ? "pressed" : "released");

    keyStatuses[key] = action == GLFW_PRESS ? JUST_PRESSED : JUST_RELEASED;
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
