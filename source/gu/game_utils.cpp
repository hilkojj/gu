#include "game_utils.h"
#include "game_config.h"
#include "profiler.h"

#include "../files/file_utils.h"
#include "../input/gamepad_input.h"
#include "../input/key_input.h"
#include "../input/mouse_input.h"
#include "../graphics/external/gl_includes.h"
#include "../graphics/external/stb_image.h"
#include "../utils/gu_error.h"

#include <imgui.h>
#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_opengl3.h"

#ifdef _WIN32
// enable dedicated graphics for NVIDIA:
extern "C"
{
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}

// enable dedicated graphics for AMD Radeon: (untested)
extern "C"
{
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

namespace gu
{

int pixelWidth = 0, pixelHeight = 0;
int virtualWidth = 0, virtualHeight = 0;
bool bFullscreen = false;
int desiredFullscreenWidth = -1;
int desiredFullscreenHeight = -1;

delegate<void(double deltaTime)> beforeRender;
delegate<void()> onResize;
delegate<bool()> canClose;

namespace
{

Config config;
GLFWwindow *window = nullptr;

Screen *screen;

bool bResized = true;
int nextVirtualWidth, nextVirtualHeight, nextPixelWidth, nextPixelHeight;

void onVirtualSizeChanged(GLFWwindow *, int width, int height)
{
    nextVirtualWidth = width;
    nextVirtualHeight = height;
}

void onPixelSizeChanged(GLFWwindow *, int width, int height)
{
    nextPixelWidth = width;
    nextPixelHeight = height;
    bResized = true;
}

void APIENTRY glMessageCallback(
    GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei length,
    const GLchar *msg, const void *data
)
{
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
    {
        if (config.bPrintOpenGLMessages)
        {
            std::cout << "====== OpenGL Message. ID: " << id << " ======" << std::endl << msg << "\n======================================" << std::endl;
        }
    }
    else if (config.bPrintOpenGLErrors)
    {
        std::cerr << "====== OpenGL Error. ID: " << id << " ======" << std::endl << msg << "\n======================================" << std::endl;
    }
}

void render(double deltaTime)
{
    if (screen)
    {
        screen->render(deltaTime);
    }
}

GLFWmonitor *getMonitorContainingWindow(GLFWwindow *window)
{
    ivec2 windowPos, windowSize;
    int numMonitors = 0;
    GLFWmonitor **monitors = glfwGetMonitors(&numMonitors);
    GLFWmonitor *monitor = nullptr;

    glfwGetWindowPos(window, &windowPos.x, &windowPos.y);
    glfwGetWindowSize(window, &windowSize.x, &windowSize.y);

    const ivec2 windowCenterPos = windowPos + windowSize / 2;

    for (int i = 0; i < numMonitors; i++) {

        monitor = monitors[i];
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);

        ivec2 monitorSize(mode->width, mode->height);
        ivec2 monitorPos;
        glfwGetMonitorPos(monitor, &monitorPos.x, &monitorPos.y);

        if (windowCenterPos.x >= monitorPos.x && windowCenterPos.x <= monitorPos.x + monitorSize.x
            && windowCenterPos.y >= monitorPos.y && windowCenterPos.y <= monitorPos.y + monitorSize.y)
        {
            break;
        }
    }

    // Fallback, even though this function never failed when testing.
    if (monitor == nullptr && numMonitors > 0)
    {
        monitor = monitors[0];
    }
    if (monitor == nullptr)
    {
        throw gu_err("No monitor found");
    }
    return monitor;
}

void updateFullscreen()
{
#ifndef EMSCRIPTEN
    static int restoreXPos = 0, restoreYPos = 0, restoreWidth = 200, restoreHeight = 200, restoreResolutionX = 1280, restoreResolutionY = 720;
    if (bFullscreen)
    {
        GLFWmonitor *monitor = glfwGetWindowMonitor(window);
        if (monitor == nullptr)
        {
            // Not fullscreen yet.
            // Save window properties
            glfwGetWindowPos(window, &restoreXPos, &restoreYPos);
            glfwGetWindowSize(window, &restoreWidth, &restoreHeight);
            // Get Monitor that window is inside, to make game fullscreen in that monitor:
            monitor = getMonitorContainingWindow(window);
            const GLFWvidmode *videoMode = glfwGetVideoMode(monitor);
            restoreResolutionX = videoMode->width;
            restoreResolutionY = videoMode->height;
        }
        const GLFWvidmode *videoMode = glfwGetVideoMode(monitor);
        int width = desiredFullscreenWidth <= 0 ? restoreResolutionX : desiredFullscreenWidth;
        int height = desiredFullscreenHeight <= 0 ? restoreResolutionY : desiredFullscreenHeight;
        glfwSetWindowMonitor(window, monitor, 0, 0, width, height, videoMode->refreshRate);
    }
    else
    {
        glfwSetWindowMonitor(window, NULL, restoreXPos, restoreYPos, restoreWidth, restoreHeight, GLFW_DONT_CARE);
    }
    glfwSwapInterval(config.bVSync);
#endif
}

#ifdef EMSCRIPTEN

EM_JS(const char *, js_getClipboardText, (),
{
    const input = window.pastedText || "";
    const lengthBytes = lengthBytesUTF8(input) + 1;
    const stringOnWasmHeap = _malloc(lengthBytes);
    stringToUTF8(input, stringOnWasmHeap, lengthBytes);
    return stringOnWasmHeap;
});
EM_JS(void, js_setClipboardText, (const char *text),
{
    window.pastedText = UTF8ToString(text);

    // Only work on secure pages (HTTPS):
    if (navigator.clipboard != undefined)
        navigator.clipboard.writeText(window.pastedText);
});

const char* EMSCRIPTENGetClipboardText(void* user_data)
{
    static std::string c;
    auto textFromJS = js_getClipboardText();
    c = textFromJS;
    free((char *)textFromJS);
    return c.c_str();
}

void EMSCRIPTENSetClipboardText(void* user_data, const char* text)
{
    js_setClipboardText(text);
}

#endif

void onGLFWError(int errorCode, const char *description)
{
    std::cerr << "GLFW ERROR: " << description << std::endl;
}

} // namespace

bool init(const Config &inConfig)
{
    config = inConfig;
    glfwSetErrorCallback(onGLFWError);
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_SAMPLES, config.samples);
    glfwWindowHint(GLFW_DEPTH_BITS, config.depthBits);

    #ifndef EMSCRIPTEN
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, config.openGLMajorVersion);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, config.openGLMinorVersion);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #endif

    window = glfwCreateWindow(config.width, config.height, config.title.c_str(), nullptr, nullptr);

    if (!window)
    {
        std::cerr << "Failed to open GLFW window." << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize OpenGL context" << std::endl;
        return false;
    }

    glfwSwapInterval(config.bVSync);

    glfwSetWindowSizeCallback(window, onVirtualSizeChanged);
    glfwSetFramebufferSizeCallback(window, onPixelSizeChanged);

    glfwGetWindowSize(window, &nextVirtualWidth, &nextVirtualHeight);
    glfwGetFramebufferSize(window, &nextPixelWidth, &nextPixelHeight);

    std::vector<GLFWimage> windowIcons;
    for (const std::string &iconPath : config.windowIconPaths)
    {
        if (fu::exists(iconPath.c_str()))
        {
            GLFWimage &image = windowIcons.emplace_back();
            int channels = 0;
            image.pixels = stbi_load(iconPath.c_str(), &image.width, &image.height, &channels, 4);
            if (channels != 4)
            {
                stbi_image_free(image.pixels);
                windowIcons.pop_back();
            }
        }
    }
    glfwSetWindowIcon(window, windowIcons.size(), windowIcons.data());
    for (GLFWimage &image : windowIcons)
    {
        stbi_image_free(image.pixels);
    }
    windowIcons.clear();

    profiler::showGUI = config.bShowProfiler;

    KeyInput::setInputWindow(window);
    MouseInput::setInputWindow(window);
    GamepadInput::setInputWindow(window);

    if (config.bPrintOpenGLMessages || config.bPrintOpenGLErrors)
    {
        #ifndef EMSCRIPTEN
        glEnable(GL_DEBUG_OUTPUT);
        if (glDebugMessageCallback)
            glDebugMessageCallback(glMessageCallback, NULL);
        else
            std::cerr << "CANNOT ENABLE OPENGL MESSAGE CALLBACKS. Disable printOpenGLErrors/printOpenGLMessages or use OpenGL >= 4.3" << std::endl;
        #endif
    }

    // IMGUI ---------------------------------
    ImGui::CreateContext();
    const char *glslVersionLine = "#version 300 es";

    // Initialize helper Platform and Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, false);
    if (config.customImGuiRendering.initialize != nullptr)
    {
        config.customImGuiRendering.initialize(glslVersionLine);
    }
    else
    {
        ImGui_ImplOpenGL3_Init(glslVersionLine);
    }
    #ifdef EMSCRIPTEN

    EM_ASM(
        document.addEventListener("paste", e => {
            window.pastedText = e.clipboardData.getData("text");
        });
    );

    ImGui::GetIO().GetClipboardTextFn = EMSCRIPTENGetClipboardText;
    ImGui::GetIO().SetClipboardTextFn = EMSCRIPTENSetClipboardText;
    #endif
    // IMGUI ---------------------------------

    #ifdef EMSCRIPTEN
    // make the WebGL context accessible in Javascript using 'window.gl' or 'gl'
    EM_ASM({
        window["gl"] = document.getElementById(UTF8ToString($0)).getContext("webgl2");

    }, config.htmlCanvasId.c_str());
    #endif

    return true;
}

double prevTime = 0.0;
int framesInSecond = 0;
double remainingSecond = 0.0;

void mainLoop()
{
    profiler::beginNewFrame();
    const double currTime = glfwGetTime();
    const double deltaTime = currTime - prevTime;

    framesInSecond++;
    if ((remainingSecond -= deltaTime) <= 0)
    {
        if (config.bShowFPSInTitleBar)
        {
            const std::string fps = std::to_string(framesInSecond) + "fps";
            glfwSetWindowTitle(window, fps.c_str());
        }
        profiler::fps = framesInSecond;
        framesInSecond = 0;
        remainingSecond = 1;
    }

    if (bResized)
    {
        virtualWidth = nextVirtualWidth;
        virtualHeight = nextVirtualHeight;
        pixelWidth = nextPixelWidth;
        pixelHeight = nextPixelHeight;
        bResized = false;
        glViewport(0, 0, nextPixelWidth, nextPixelHeight);
        if (screen)
            screen->onResize();
        onResize();
    }

    // Feed inputs to dear imgui, start new frame
    if (config.customImGuiRendering.newFrame != nullptr)
    {
        config.customImGuiRendering.newFrame();
    }
    else
    {
        ImGui_ImplOpenGL3_NewFrame();
    }
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {
        profiler::Zone z("logic");
        beforeRender(min(deltaTime, .1));
    } {
        profiler::Zone z("render");
        render(min(deltaTime, .1));
        static bool wasFullscreen = false;
        static int prevDesiredFullscreenWidth = -1;
        static int prevDesiredFullscreenHeight = -1;
        if (wasFullscreen != bFullscreen)
        {
            updateFullscreen();
        }
        else if (bFullscreen && (prevDesiredFullscreenWidth != desiredFullscreenWidth || prevDesiredFullscreenHeight != desiredFullscreenHeight))
        {
            updateFullscreen();
        }
        wasFullscreen = bFullscreen;
        prevDesiredFullscreenWidth = desiredFullscreenWidth;
        prevDesiredFullscreenHeight = desiredFullscreenHeight;
    } {
        profiler::Zone z("input");
        KeyInput::update();
        MouseInput::update();
        GamepadInput::update();
    }

    profiler::frames.back().time = glfwGetTime() - currTime;
    if (profiler::showGUI) profiler::drawProfilerImGUI();

    // Render dear imgui into screen
    ImGui::Render();
    if (config.customImGuiRendering.drawData != nullptr)
    {
        config.customImGuiRendering.drawData(ImGui::GetDrawData());
    }
    else
    {
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();

    prevTime = currTime;
}

void run()
{
    prevTime = glfwGetTime();
    framesInSecond = 0;
    remainingSecond = 1;

    #ifdef EMSCRIPTEN
    emscripten_set_main_loop(mainLoop, config.bVSync ? 0 : 99999, 1);
    #else

    do mainLoop();
    while (!shouldClose() || canClose().anyEquals(false));

    ImGui_ImplGlfw_Shutdown();

    #endif
    // glfwTerminate();
}

bool shouldClose()
{
    return glfwWindowShouldClose(window);
}

void setShouldClose(bool val)
{
    glfwSetWindowShouldClose(window, val);
}

void setScreen(Screen *newScreen)
{
    screen = newScreen;
    onResize();
}

bool getVSync()
{
    return config.bVSync;
}

void setVSync(bool bEnabled)
{
    if (bEnabled == config.bVSync)
        return;

    config.bVSync = bEnabled;
    glfwSwapInterval(bEnabled);
}

} // namespace gu
