#ifndef GAME_UTILS_H
#define GAME_UTILS_H

#include <iostream>
#include <functional>
#include "../gl_includes.h"
#include "../input/key_input.h"
#include "../input/mouse_input.h"
#include "screen.h"
#include "profiler.h"

namespace gu
{

struct Config
{
    std::string title = "Game";
    int width = 1600, height = 900, samples = 4;
    bool 
        vsync = false,
        showFPSInTitleBar = false,
        showProfiler = true,
        printOpenGLMessages = false,
        printOpenGLErrors = true;

    // the id of the HTML <canvas> element:
    std::string htmlCanvasId = "canvas";
};

extern Config config;
extern GLFWwindow *window;
extern int width, height, widthPixels, heightPixels;
extern bool fullscreen;
extern std::function<void(double deltaTime)> beforeRender;

// initialize OpenGL, window, input and more. Returns false if initialization failed.
bool init(Config config);
void run();
void setScreen(Screen *screen);
void setVSync(bool enabled);

}; // namespace gu

#endif
