
#ifndef KEY_INPUT_H
#define KEY_INPUT_H

#include <iostream>
#include <map>

#include "GLFW/glfw3.h"
#include "../json.hpp"

namespace KeyInput
{

void setInputWindow(GLFWwindow* window);

void update();

bool justPressed(int key);

bool pressed(int key);

bool justReleased(int key);

extern const std::map<int, const char *> KEY_NAMES;

const char *getKeyName(int key);

/**
 * WARNING: very inefficient function..
 */
int getKeyByName(const char *keyName);

struct Key
{
    int glfwValue = GLFW_KEY_UNKNOWN;

    Key() = default;
    Key(const int &v) : glfwValue(v) {}

    operator int() const { return glfwValue; }
};

void to_json(json& j, const Key& v);

void from_json(const json& j, Key& v);

} // namespace KeyInput

#endif
