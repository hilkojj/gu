
#ifndef GU_CONFIG_H
#define GU_CONFIG_H

#include <string>
#include <vector>

struct ImDrawData;


namespace gu
{

struct Config
{
    std::string title = "Game";
    int width = 1600, height = 900, samples = 0, depthBits = 24;
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

#endif
