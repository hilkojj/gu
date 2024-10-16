#ifndef GAME_UTILS_H
#define GAME_UTILS_H

#include "screen.h"

#include "../utils/delegate.h"

namespace gu
{

// Window content size in pixels. Prefer this over virtual size when using OpenGL functions.
extern int pixelWidth, pixelHeight;

// Window content size in screen coordinates. https://www.glfw.org/docs/3.3/window_guide.html#window_size
extern int virtualWidth, virtualHeight;

extern bool bFullscreen;

extern delegate<void(double deltaTime)> beforeRender;
extern delegate<void()> onResize;
extern delegate<bool()> canClose;

// initialize OpenGL, window, input and more. Returns false if initialization failed.
bool init(const struct Config &config);

void run();

bool shouldClose();

void setShouldClose(bool);

void setScreen(Screen *screen);

bool getVSync();

void setVSync(bool bEnabled);

}; // namespace gu

#endif
