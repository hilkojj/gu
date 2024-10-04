#ifndef GAME_UTILS_H
#define GAME_UTILS_H

#include "screen.h"
#include "profiler.h"

#include "../graphics/external/gl_includes.h"
#include "../input/key_input.h"
#include "../input/mouse_input.h"
#include "../input/gamepad_input.h"
#include "../utils/delegate.h"

#include <iostream>
#include <functional>

struct ImDrawData;

namespace gu
{

struct Config
{
    std::string title = "Game";
    int width = 1600, height = 900, samples = 4, depthBits = 24;
    bool 
        vsync = false,
        showFPSInTitleBar = false,
        showProfiler = true,
        printOpenGLMessages = false,
        printOpenGLErrors = true;

    // the id of the HTML <canvas> element:
    std::string htmlCanvasId = "canvas";

    int openGLMajorVersion = 4, openGLMinorVersion = 2;

    bool (*customImGuiRenderingInit)(const char *glslVersion) = nullptr;

    void (*customImGuiRenderingNewFrame)() = nullptr;

    void (*customImGuiRenderingRenderDrawData)(ImDrawData *) = nullptr;
};

extern Config config;
extern GLFWwindow *window;
extern int width, height, widthPixels, heightPixels;
extern bool fullscreen;
extern std::function<void(double deltaTime)> beforeRender;
extern delegate<bool()> canClose;
extern delegate<void()> onResize;

// initialize OpenGL, window, input and more. Returns false if initialization failed.
bool init(Config config);
void run();
bool shouldClose();
void setShouldClose(bool);
void setScreen(Screen *screen);
void setVSync(bool enabled);

}; // namespace gu

#endif
