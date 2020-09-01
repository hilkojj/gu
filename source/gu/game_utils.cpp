#include "game_utils.h"
#include "../utils/math_utils.h"
#include "../utils/gu_error.h"
#include <string>

#include "imgui.h"
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

Config config;
GLFWwindow *window = nullptr;

// size of the window:
// width & height are in screen coordinates, widthPixels & heightPixels are in pixels.
int width = 0, height = 0, widthPixels = 0, heightPixels = 0;
bool fullscreen = false;

std::function<void(double)> beforeRender = [](auto){};

delegate<bool()> canClose;
delegate<void()> onResize;

namespace
{
Screen *screen;

void APIENTRY glMessageCallback(GLenum source, GLenum type, GLuint id,
                            GLenum severity, GLsizei length,
                            const GLchar *msg, const void *data)
{
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
    {
        if (config.printOpenGLMessages)
            std::cout << "====== OpenGL Message. ID: " << id << " ======" << std::endl << msg << "\n======================================" << std::endl;
    }
    else if (config.printOpenGLErrors)
        std::cerr << "====== OpenGL Error. ID: " << id << " ======" << std::endl << msg << "\n======================================" << std::endl;
}

bool resized = true;
int nextWidth, nextHeight, nextWidthPixels, nextHeightPixels;

void window_size_callback(GLFWwindow* window, int width, int height)
{
    nextWidth = width;
    nextHeight = height;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    nextWidthPixels = width;
    nextHeightPixels = height;
    resized = true;
}

void render(double deltaTime)
{
    if (screen)
        screen->render(deltaTime);
}

GLFWmonitor *get_current_monitor(GLFWwindow *window)
{
    ivec2 windowPos, windowSize;
    int nrOfMonitors;
    GLFWmonitor **monitors = glfwGetMonitors(&nrOfMonitors);
    GLFWmonitor *monitor = NULL;

    glfwGetWindowPos(window, &windowPos.x, &windowPos.y);
    glfwGetWindowSize(window, &windowSize.x, &windowSize.y);

    ivec2 winCenter = windowPos + windowSize / 2;

    for (int i = 0; i < nrOfMonitors; i++) {

        monitor = monitors[i];
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);

        ivec2 monitorSize(mode->width, mode->height);
        ivec2 monitorPos;
        glfwGetMonitorPos(monitor, &monitorPos.x, &monitorPos.y);

        if (
                winCenter.x >= monitorPos.x && winCenter.x <= monitorPos.x + monitorSize.x
                &&
                winCenter.y >= monitorPos.y && winCenter.y <= monitorPos.y + monitorSize.y
        )
            break;
    }
    if (!monitor) throw gu_err("No monitor found");
    return monitor;
}

void toggleFullscreen()
{
    #ifndef EMSCRIPTEN
    static int restoreXPos, restoreYPos, restoreWidth, restoreHeight;
    if (fullscreen)
    {
        glfwGetWindowPos(window, &restoreXPos, &restoreYPos);
        glfwGetWindowSize(window, &restoreWidth, &restoreHeight);
        auto monitor = get_current_monitor(window);

        auto videoMode = glfwGetVideoMode(monitor);
        int w = videoMode->width, h = videoMode->height;
        glfwSetWindowMonitor(window, monitor, 0, 0, w, h, videoMode->refreshRate);
    }
    else
    {
        glfwSetWindowMonitor(window, NULL, restoreXPos, restoreYPos, restoreWidth, restoreHeight, GLFW_DONT_CARE);
    }
    glfwSwapInterval(config.vsync);
    #endif
}

#ifdef EMSCRIPTEN

EM_JS(const char *, js_getClipboardText, (), {

    var input = window.pastedText || "";
    var lengthBytes = lengthBytesUTF8(input) + 1;
    var stringOnWasmHeap = _malloc(lengthBytes);
    stringToUTF8(input, stringOnWasmHeap, lengthBytes);
    return stringOnWasmHeap;
});
EM_JS(void, js_setClipboardText, (const char *text), {

    window.pastedText = UTF8ToString(text);

    // ONLY works in HTTPS mode:
    if (navigator.clipboard != undefined)
        navigator.clipboard.writeText(window.pastedText);

    // hacky fallback for HTTP:
    else setTimeout(() => {
        var textArea = document.createElement("textarea");
        textArea.value = window.pastedText;

        // Avoid scrolling to bottom
        textArea.style.top = "0";
        textArea.style.left = "0";
        textArea.style.position = "fixed";

        document.body.appendChild(textArea);
        textArea.focus();
        textArea.select();

        try {
            if (!document.execCommand('copy'))
                console.warn('Could not copy text');
        } catch (err) {
            console.error('Unable to copy:', err);
        }
        document.body.removeChild(textArea);
    })
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

} // namespace

bool init(Config config_)
{
    config = config_;
    // Initialise GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_SAMPLES, config.samples);

    #ifndef EMSCRIPTEN
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL
    #endif

    window = glfwCreateWindow(config.width, config.height, config.title.c_str(), NULL, NULL);

    if (!window)
    {
        std::cerr << "Failed to open GLFW window." << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize OpenGL context" << std::endl;
        return false;
    }

    if (config.printOpenGLMessages || config.printOpenGLErrors)
    {
        #ifndef EMSCRIPTEN
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(glMessageCallback, NULL);
        #endif
    }
    glfwSwapInterval(config.vsync);

    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwGetWindowSize(window, &nextWidth, &nextHeight);
    glfwGetFramebufferSize(window, &nextWidthPixels, &nextHeightPixels);

    profiler::showGUI = config.showProfiler;

    KeyInput::setInputWindow(window);
    MouseInput::setInputWindow(window);

    // IMGUI ---------------------------------
    ImGui::CreateContext();
    const char* glsl_version = "#version 300 es";

    // Initialize helper Platform and Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, false);
    ImGui_ImplOpenGL3_Init(glsl_version);
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

double prevTime;
int framesInSecond;
double remainingSecond;

void mainLoop()
{
    profiler::beginNewFrame();
    double currTime = glfwGetTime();
    double deltaTime = currTime - prevTime;

    framesInSecond++;
    if ((remainingSecond -= deltaTime) <= 0)
    {
        if (config.showFPSInTitleBar)
        {
            std::string fps = std::to_string(framesInSecond) + "fps";
            glfwSetWindowTitle(window, fps.c_str());
        }
        profiler::fps = framesInSecond;
        framesInSecond = 0;
        remainingSecond = 1;
    }

    if (resized)
    {
        width = nextWidth;
        height = nextHeight;
        widthPixels = nextWidthPixels;
        heightPixels = nextHeightPixels;
        resized = false;
        glViewport(0, 0, nextWidthPixels, nextHeightPixels);
        if (screen)
            screen->onResize();
        onResize();
    }

    // Feed inputs to dear imgui, start new frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {
        profiler::Zone z("logic");
        beforeRender(min(deltaTime, .1));
    } {
        profiler::Zone z("render");
        render(min(deltaTime, .1));
        static bool wasFullscreen = false;
        if (wasFullscreen != fullscreen)
            toggleFullscreen();
        wasFullscreen = fullscreen;
    } {
        profiler::Zone z("input");
        KeyInput::update();
        MouseInput::update();
    }

    profiler::frames.back().time = glfwGetTime() - currTime;
    if (profiler::showGUI) profiler::drawProfilerImGUI();

    // Render dear imgui into screen
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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
    emscripten_set_main_loop(mainLoop, config.vsync ? 0 : 99999, 1);
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

void setVSync(bool enabled)
{
    if (enabled == config.vsync)
        return;

    config.vsync = enabled;
    glfwSwapInterval(enabled);
}

} // namespace gu
