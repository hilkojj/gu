#include "key_input.h"

#include "../utils/gu_error.h"
#include "../json.hpp"

#include <imgui.h>
#include <examples/imgui_impl_glfw.h>
#include <map>

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

void keyCallbackWithoutImGui(int key, int action)
{
    if (action == GLFW_REPEAT || (ImGui::GetIO().WantCaptureKeyboard && action != GLFW_RELEASE))
    {
        return;
    }

    // printf("%s was %s\n", glfwGetKeyName(key, 0), action == GLFW_PRESS ? "pressed" : "released");

    KeyStatus &status = keyStatuses[key];

    if (action == GLFW_RELEASE && status == NOT_PRESSED)
    {
        return;
    }

    status = action == GLFW_PRESS ? JUST_PRESSED : JUST_RELEASED;
}

void glfwCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

#ifndef DO_NOT_COMBINE_RIGHT_AND_LEFT_MODIFIER_KEYS
    if (key >= GLFW_KEY_RIGHT_SHIFT && key <= GLFW_KEY_RIGHT_SUPER)
    {
        keyCallbackWithoutImGui(key - (GLFW_KEY_RIGHT_SHIFT - GLFW_KEY_LEFT_SHIFT), action);
    }
#endif
    keyCallbackWithoutImGui(key, action);
}

} // namespace

void setInputWindow(GLFWwindow *window)
{
    glfwSetKeyCallback(window, KeyInput::glfwCallback);
    glfwSetCharCallback(window, ImGui_ImplGlfw_CharCallback);
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

const char *getKeyName(int key)
{
    return KEY_NAMES.find(key) != KEY_NAMES.end() ? KEY_NAMES.at(key) : "Unknown";
}

int getKeyByName(const char *keyName)
{
    for (auto &[key, name] : KEY_NAMES)
        if (strcmp(keyName, name) == 0)
            return key;

    throw gu_err("I looked very hard at my keyboard, but I really could NOT find a key named '" + std::string(keyName) + "'");
}

void to_json(json &j, const Key &v)
{

    j = KeyInput::getKeyName(v.glfwValue);
}

void from_json(const json &j, Key &v)
{

    v.glfwValue = KeyInput::getKeyByName(std::string(j).c_str());
}

} // namespace KeyInput
