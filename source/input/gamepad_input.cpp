#include "gamepad_input.h"
#include "gamepad_input.h"
#include "gamepad_input.h"
#include "gamepad_input.h"

#include "gamepad_input.h"
#include <iostream>
#include <utils/gu_error.h>

namespace GamepadInput
{

delegate<void(uint gamepad, const std::string &name)> &getOnConnected()
{
    static delegate<void(uint, const std::string &)> d;
    return d;
}

delegate<void(uint gamepad, const std::string &name)> &getOnDisconnected()
{
    static delegate<void(uint, const std::string &)> d;
    return d;
}

namespace
{

    static std::map<uint, std::string> gamepads;

    static std::map<uint, GLFWgamepadstate[2]> states;

    void joystick_callback(int jid, int event)
    {
        if (!glfwJoystickIsGamepad(jid))
            return;

        if (event == GLFW_CONNECTED)
        {
            // The joystick was connected

            std::string joystickName = glfwGetJoystickName(jid);
            std::string gamepadName = glfwGetGamepadName(jid);

            gamepads[jid] = joystickName;

            std::cout << "Gamepad " << jid << " '" << joystickName << "'/'" << gamepadName << "' connected." << std::endl;
            getOnConnected()(jid, joystickName);
        }
        else if (event == GLFW_DISCONNECTED)
        {
            // The joystick was disconnected

            auto it = gamepads.find(jid);
            if (it != gamepads.end())
            {
                std::string name = it->second;
                gamepads.erase(jid);

                std::cout << "Gamepad " << jid << " '" << name << "' disconnected." << std::endl;
                getOnDisconnected()(jid, name);
            }
        }
    }

}

void setInputWindow(GLFWwindow *window)
{
    glfwSetJoystickCallback(joystick_callback);
}

void update()
{

    for (int jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_16; jid++)
    {
        if (glfwJoystickPresent(jid))
        {
            if (gamepads.find(jid) == gamepads.end())
            {
                joystick_callback(jid, GLFW_CONNECTED);
            }

            states[jid][0] = states[jid][1];
            glfwGetGamepadState(jid, &states[jid][1]);
            
            for (int axis = GLFW_GAMEPAD_AXIS_LEFT_X; axis < GLFW_GAMEPAD_AXIS_LAST; axis += 2)
            {
                vec2 stick(
                    states[jid][1].axes[axis],
                    states[jid][1].axes[axis + 1]
                );

                // https://www.gamedeveloper.com/disciplines/doing-thumbstick-dead-zones-right

                static const float DEADZONE = .25f;

                auto len = length(stick);

                if (len < DEADZONE)
                {
                    states[jid][1].axes[axis] = 0;
                    states[jid][1].axes[axis + 1] = 0;
                }
                else
                {
                    stick = normalize(stick) * ((len - DEADZONE) / (1.f - DEADZONE));
                }
            }
        }
        else
        {
            if (gamepads.find(jid) != gamepads.end())
            {
                joystick_callback(jid, GLFW_DISCONNECTED);
            }
        }
    }

}

const std::map<uint, std::string> &getGamepads()
{
    return gamepads;
}

bool justPressed(uint gamepad, uint button)
{
    if (button > GLFW_GAMEPAD_BUTTON_LAST)
    {
        throw gu_err(std::to_string(button) + " > GLFW_GAMEPAD_BUTTON_LAST. For gamepad " + std::to_string(gamepad));
    }
    return states[gamepad][1].buttons[button] && !states[gamepad][0].buttons[button];
}

bool pressed(uint gamepad, uint button)
{
    if (button > GLFW_GAMEPAD_BUTTON_LAST)
    {
        throw gu_err(std::to_string(button) + " > GLFW_GAMEPAD_BUTTON_LAST. For gamepad " + std::to_string(gamepad));
    }
    return states[gamepad][1].buttons[button];
}

bool justReleased(uint gamepad, uint button)
{
    if (button > GLFW_GAMEPAD_BUTTON_LAST)
    {
        throw gu_err(std::to_string(button) + " > GLFW_GAMEPAD_BUTTON_LAST. For gamepad " + std::to_string(gamepad));
    }
    return !states[gamepad][1].buttons[button] && states[gamepad][0].buttons[button];
}

float getAxis(uint gamepad, uint axis)
{
    if (axis > GLFW_GAMEPAD_AXIS_LAST)
    {
        throw gu_err(std::to_string(axis) + " > GLFW_GAMEPAD_AXIS_LAST. For gamepad " + std::to_string(gamepad));
    }
    return states[gamepad][1].axes[axis];
}

const char *getButtonName(uint button)
{
    return BUTTON_NAMES.find(button) != BUTTON_NAMES.end() ? BUTTON_NAMES.at(button) : "Unknown";
}

int getButtonByName(const char *buttonName)
{
    for (auto &[key, name] : BUTTON_NAMES)
        if (strcmp(buttonName, name) == 0)
            return key;

    throw gu_err("No gamepad button named '" + std::string(buttonName) + "'");
}

const char *getAxisName(uint axis)
{
    return AXIS_NAMES.find(axis) != AXIS_NAMES.end() ? AXIS_NAMES.at(axis) : "Unknown";
}

int getAxisByName(const char *axisName)
{
    for (auto &[key, name] : AXIS_NAMES)
        if (strcmp(axisName, name) == 0)
            return key;

    throw gu_err("No gamepad axis named '" + std::string(axisName) + "'");
}

void to_json(json &j, const Button &v)
{
    j = getButtonName(v.glfwValue);
}

void from_json(const json &j, Button &v)
{
    v.glfwValue = getButtonByName(std::string(j).c_str());
}

void to_json(json &j, const Axis &v)
{
    j = getAxisName(v.glfwValue);
}

void from_json(const json &j, Axis &v)
{
    v.glfwValue = getAxisByName(std::string(j).c_str());
}


}
