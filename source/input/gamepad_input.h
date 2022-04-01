
#ifndef GAMEPAD_INPUT_H
#define GAMEPAD_INPUT_H

#include "GLFW/glfw3.h"
#include "../utils/delegate.h"
#include <map>
#include <string>
#include "../utils/math_utils.h"
#include "../json.hpp"

namespace GamepadInput
{
delegate<void(uint gamepad, const std::string &name)> &getOnConnected();

delegate<void(uint gamepad, const std::string &name)> &getOnDisconnected();

void setInputWindow(GLFWwindow *);

void update();

const std::map<uint, std::string> &getGamepads();

bool justPressed(uint gamepad, uint button);

bool pressed(uint gamepad, uint button);

bool justReleased(uint gamepad, uint button);

float getAxis(uint gamepad, uint axis);


inline const std::map<uint, const char *> BUTTON_NAMES = {
        {GLFW_GAMEPAD_BUTTON_A, "A"},
        {GLFW_GAMEPAD_BUTTON_B, "B"},
        {GLFW_GAMEPAD_BUTTON_X, "X"},
        {GLFW_GAMEPAD_BUTTON_Y, "Y"},
        {GLFW_GAMEPAD_BUTTON_LEFT_BUMPER, "Left Bumper"},
        {GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER, "Right Bumper"},
        {GLFW_GAMEPAD_BUTTON_BACK, "Back"},
        {GLFW_GAMEPAD_BUTTON_START, "Start"},
        {GLFW_GAMEPAD_BUTTON_GUIDE, "Guide"},
        {GLFW_GAMEPAD_BUTTON_LEFT_THUMB, "Left Thumb"},
        {GLFW_GAMEPAD_BUTTON_RIGHT_THUMB, "Right Thumb"},
        {GLFW_GAMEPAD_BUTTON_DPAD_UP, "D-pad Up"},
        {GLFW_GAMEPAD_BUTTON_DPAD_RIGHT, "D-pad Right"},
        {GLFW_GAMEPAD_BUTTON_DPAD_DOWN, "D-pad Down"},
        {GLFW_GAMEPAD_BUTTON_DPAD_LEFT, "D-pad Left"},
};

inline const std::map<uint, const char *> AXIS_NAMES = {
        {GLFW_GAMEPAD_AXIS_LEFT_X, "Left Joystick X"},
        {GLFW_GAMEPAD_AXIS_LEFT_Y, "Left Joystick Y"},
        {GLFW_GAMEPAD_AXIS_RIGHT_X, "Right Joystick X"},
        {GLFW_GAMEPAD_AXIS_RIGHT_Y, "Right Joystick Y"},
        {GLFW_GAMEPAD_AXIS_LEFT_TRIGGER, "Left Trigger"},
        {GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER, "Right Trigger"},
};

const char *getButtonName(uint button);

int getButtonByName(const char *buttonName);

const char *getAxisName(uint axis);

int getAxisByName(const char *axisName);

struct Button
{
    uint glfwValue = 0;;

    Button() = default;
    Button(const uint &v) : glfwValue(v) {}

    operator int() const { return glfwValue; }
};

void to_json(json &j, const Button &v);

void from_json(const json &j, Button &v);

struct Axis
{
    uint glfwValue = 0;;

    Axis() = default;
    Axis(const uint &v) : glfwValue(v) {}

    operator int() const { return glfwValue; }
};

void to_json(json &j, const Axis &v);

void from_json(const json &j, Axis &v);

}

#endif
