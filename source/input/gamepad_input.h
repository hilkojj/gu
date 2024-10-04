
#ifndef GAMEPAD_INPUT_H
#define GAMEPAD_INPUT_H

#include "../graphics/external/gl_includes.h"
#include "../utils/delegate.h"
#include "../math/math_utils.h"
#include "../json.hpp"

#include <map>
#include <string>

#ifdef EMSCRIPTEN
// emscripten does not have the GLFW gamepad api implemented, it does have the joystick api implemented, which should be enough.

// original glfw button index,              javascript button index
#define GLFW_GAMEPAD_BUTTON_A               0
#define GLFW_GAMEPAD_BUTTON_B               1
#define GLFW_GAMEPAD_BUTTON_X               2
#define GLFW_GAMEPAD_BUTTON_Y               3
#define GLFW_GAMEPAD_BUTTON_LEFT_BUMPER     4
#define GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER    5
#define GLFW_GAMEPAD_BUTTON_BACK            8
#define GLFW_GAMEPAD_BUTTON_START           9
#define GLFW_GAMEPAD_BUTTON_GUIDE           16
#define GLFW_GAMEPAD_BUTTON_LEFT_THUMB      10
#define GLFW_GAMEPAD_BUTTON_RIGHT_THUMB     11
#define GLFW_GAMEPAD_BUTTON_DPAD_UP         12
#define GLFW_GAMEPAD_BUTTON_DPAD_RIGHT      15
#define GLFW_GAMEPAD_BUTTON_DPAD_DOWN       13
#define GLFW_GAMEPAD_BUTTON_DPAD_LEFT       14

#define GLFW_GAMEPAD_BUTTON_LAST            GLFW_GAMEPAD_BUTTON_GUIDE


// original glfw axis index,            javascript axis index
#define GLFW_GAMEPAD_AXIS_LEFT_X        0
#define GLFW_GAMEPAD_AXIS_LEFT_Y        1
#define GLFW_GAMEPAD_AXIS_RIGHT_X       2
#define GLFW_GAMEPAD_AXIS_RIGHT_Y       3

// these have no purpose:
#define GLFW_GAMEPAD_AXIS_FAKE_X        4
#define GLFW_GAMEPAD_AXIS_FAKE_Y        5

// these are floating-point buttons in Javascript, instead of axes. So I gave them the js button index:
#define GLFW_GAMEPAD_AXIS_LEFT_TRIGGER  6
#define GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER 7

#define GLFW_GAMEPAD_AXIS_LAST          GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER


#endif // EMSCRIPTEN


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
