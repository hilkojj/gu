
#include <map>
#include <imgui.h>
#include <examples/imgui_impl_glfw.h>
#include <utils/gu_error.h>
#include "key_input.h"

namespace KeyInput
{

const std::map<int, const char *> KEY_NAMES = {
    {GLFW_KEY_UNKNOWN, "Unknown"},

    {GLFW_KEY_SPACE, "Space"},
    {GLFW_KEY_APOSTROPHE, "'"},
    {GLFW_KEY_COMMA, ","},
    {GLFW_KEY_MINUS, "-"},
    {GLFW_KEY_PERIOD, "."},
    {GLFW_KEY_SLASH, "/"},
    {GLFW_KEY_0, "0"},
    {GLFW_KEY_1, "1"},
    {GLFW_KEY_2, "2"},
    {GLFW_KEY_3, "3"},
    {GLFW_KEY_4, "4"},
    {GLFW_KEY_5, "5"},
    {GLFW_KEY_6, "6"},
    {GLFW_KEY_7, "7"},
    {GLFW_KEY_8, "8"},
    {GLFW_KEY_9, "9"},
    {GLFW_KEY_SEMICOLON, ";"},
    {GLFW_KEY_EQUAL, "="},
    {GLFW_KEY_A, "A"},
    {GLFW_KEY_B, "B"},
    {GLFW_KEY_C, "C"},
    {GLFW_KEY_D, "D"},
    {GLFW_KEY_E, "E"},
    {GLFW_KEY_F, "F"},
    {GLFW_KEY_G, "G"},
    {GLFW_KEY_H, "H"},
    {GLFW_KEY_I, "I"},
    {GLFW_KEY_J, "J"},
    {GLFW_KEY_K, "K"},
    {GLFW_KEY_L, "L"},
    {GLFW_KEY_M, "M"},
    {GLFW_KEY_N, "N"},
    {GLFW_KEY_O, "O"},
    {GLFW_KEY_P, "P"},
    {GLFW_KEY_Q, "Q"},
    {GLFW_KEY_R, "R"},
    {GLFW_KEY_S, "S"},
    {GLFW_KEY_T, "T"},
    {GLFW_KEY_U, "U"},
    {GLFW_KEY_V, "V"},
    {GLFW_KEY_W, "W"},
    {GLFW_KEY_X, "X"},
    {GLFW_KEY_Y, "Y"},
    {GLFW_KEY_Z, "Z"},
    {GLFW_KEY_LEFT_BRACKET, "["},
    {GLFW_KEY_BACKSLASH, "\\"},
    {GLFW_KEY_RIGHT_BRACKET, "]"},
    {GLFW_KEY_GRAVE_ACCENT, "`"},
    {GLFW_KEY_WORLD_1, "non-US #1"},
    {GLFW_KEY_WORLD_2, "non-US #2"},

    {GLFW_KEY_ESCAPE, "ESC"},
    {GLFW_KEY_ENTER, "Enter"},
    {GLFW_KEY_TAB, "Tab"},
    {GLFW_KEY_BACKSPACE, "Backspace"},
    {GLFW_KEY_INSERT, "Insert"},
    {GLFW_KEY_DELETE, "Delete"},
    {GLFW_KEY_RIGHT, "Right Arrow"},
    {GLFW_KEY_LEFT, "Left Arrow"},
    {GLFW_KEY_DOWN, "Down Arrow"},
    {GLFW_KEY_UP, "Up Arrow"},
    {GLFW_KEY_PAGE_UP, "Page Up"},
    {GLFW_KEY_PAGE_DOWN, "Page Down"},
    {GLFW_KEY_HOME, "Home"},
    {GLFW_KEY_END, "End"},
    {GLFW_KEY_CAPS_LOCK, "Caps Lock"},
    {GLFW_KEY_SCROLL_LOCK, "Scroll Lock"},
    {GLFW_KEY_NUM_LOCK, "Num Lock"},
    {GLFW_KEY_PRINT_SCREEN, "Print Screen"},
    {GLFW_KEY_PAUSE, "Pause"},
    {GLFW_KEY_F1, "F1"},
    {GLFW_KEY_F2, "F2"},
    {GLFW_KEY_F3, "F3"},
    {GLFW_KEY_F4, "F4"},
    {GLFW_KEY_F5, "F5"},
    {GLFW_KEY_F6, "F6"},
    {GLFW_KEY_F7, "F7"},
    {GLFW_KEY_F8, "F8"},
    {GLFW_KEY_F9, "F9"},
    {GLFW_KEY_F10, "F10"},
    {GLFW_KEY_F11, "F11"},
    {GLFW_KEY_F12, "F12"},
    {GLFW_KEY_F13, "F13"},
    {GLFW_KEY_F14, "F14"},
    {GLFW_KEY_F15, "F15"},
    {GLFW_KEY_F16, "F16"},
    {GLFW_KEY_F17, "F17"},
    {GLFW_KEY_F18, "F18"},
    {GLFW_KEY_F19, "F19"},
    {GLFW_KEY_F20, "F20"},
    {GLFW_KEY_F21, "F21"},
    {GLFW_KEY_F22, "F22"},
    {GLFW_KEY_F23, "F23"},
    {GLFW_KEY_F24, "F24"},
    {GLFW_KEY_F25, "F25"},
    {GLFW_KEY_KP_0, "KeyPad 0"},
    {GLFW_KEY_KP_1, "KeyPad 1"},
    {GLFW_KEY_KP_2, "KeyPad 2"},
    {GLFW_KEY_KP_3, "KeyPad 3"},
    {GLFW_KEY_KP_4, "KeyPad 4"},
    {GLFW_KEY_KP_5, "KeyPad 5"},
    {GLFW_KEY_KP_6, "KeyPad 6"},
    {GLFW_KEY_KP_7, "KeyPad 7"},
    {GLFW_KEY_KP_8, "KeyPad 8"},
    {GLFW_KEY_KP_9, "KeyPad 9"},
    {GLFW_KEY_KP_DECIMAL, "KeyPad Decimal"},
    {GLFW_KEY_KP_DIVIDE, "KeyPad Divide"},
    {GLFW_KEY_KP_MULTIPLY, "KeyPad Multiply"},
    {GLFW_KEY_KP_SUBTRACT, "KeyPad Subtract"},
    {GLFW_KEY_KP_ADD, "KeyPad Add"},
    {GLFW_KEY_KP_ENTER, "KeyPad Enter"},
    {GLFW_KEY_KP_EQUAL, "KeyPad Equal"},
    {GLFW_KEY_LEFT_SHIFT, "Shift"},
    {GLFW_KEY_LEFT_CONTROL, "CTRL"},
    {GLFW_KEY_LEFT_ALT, "ALT"},
    {GLFW_KEY_LEFT_SUPER, "Super"},
    {GLFW_KEY_RIGHT_SHIFT, "Right Shift"},
    {GLFW_KEY_RIGHT_CONTROL, "Right CTRL"},
    {GLFW_KEY_RIGHT_ALT, "Right ALT"},
    {GLFW_KEY_RIGHT_SUPER, "Right Super"},
    {GLFW_KEY_MENU, "Menu"},
};

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

void glfwCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

    if (action == GLFW_REPEAT || ImGui::GetIO().WantCaptureKeyboard)
        return;

    // printf("%s was %s\n", glfwGetKeyName(key, 0), action == GLFW_PRESS ? "pressed" : "released");

    keyStatuses[key] = action == GLFW_PRESS ? JUST_PRESSED : JUST_RELEASED;
}

} // namespace

void setInputWindow(GLFWwindow* window)
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
