#pragma once

#include <string>
#include <vector>

struct ImDrawData;


namespace gu
{

struct Config
{
    std::string title = "Game";

    // Requested window size at startup is in screen coordinates, not pixels.
    int width = 1600, height = 900;

    int samples = 0, depthBits = 24;
    bool
        bVSync = false,
        bShowFPSInTitleBar = false,
        bShowProfiler = true,
        bPrintOpenGLMessages = false,
        bPrintOpenGLErrors = true;

    int openGLMajorVersion = 4, openGLMinorVersion = 2;

    // For web-builds: the id of the HTML <canvas> element
    std::string htmlCanvasId = "canvas";

    // Paths to image files of different sizes to be selected as window icon. Common sizes are 16x16, 32x32 and 48x48
    std::vector<std::string> windowIconPaths;

    struct
    {
        bool (*initialize)(const char *glslVersionLine) = nullptr;

        void (*newFrame)() = nullptr;

        void (*drawData)(ImDrawData *) = nullptr;
    }
    customImGuiRendering;
};

}
