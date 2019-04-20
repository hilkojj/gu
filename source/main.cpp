#include <iostream>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "game/game.h"

#include "test_screens/triangle_screen.cpp"
#include "test_screens/mesh_screen.cpp"
#include "../external/json.hpp"
#include "files/file.h"
using json = nlohmann::json;

void APIENTRY glMessageCallback(GLenum source, GLenum type, GLuint id,
                            GLenum severity, GLsizei length,
                            const GLchar *msg, const void *data)
{
    std::cerr << "\n====== OpenGL Message. ID: " << id << " ======" << std::endl << msg << std::endl << "======================================\n\n";
}

int main()
{
    // json j = json::parse(File::readString("assets/models/example.g3dj"));
    // json j = json::from_ubjson(File::readBinary("assets/models/example.ubj"));

    // Initialise GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL

    GLFWwindow *window;
    window = glfwCreateWindow(1600, 900, "Game", NULL, NULL);
    
    if (!window)
    {
        std::cerr << "Failed to open GLFW window.\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize OpenGL context\n";
        return -1;
    }

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(glMessageCallback, NULL);

    glfwSwapInterval(0); // a way to disable vsync.

    glViewport(0, 0, 1600, 900);

    double prevTime = glfwGetTime();
    int framesInSecond = 0;
    double remainingSecond = 1;

    MeshScreen scr;
    Game::setScreen(&scr);

    do
    {
        double currTime = glfwGetTime();
        double deltaTime = currTime - prevTime;

        framesInSecond++;

        if ((remainingSecond -= deltaTime) <= 0) {
            std::string fps = std::to_string(framesInSecond) + "fps";
            glfwSetWindowTitle(window, fps.c_str());
            framesInSecond = 0;
            remainingSecond = 1;
        }

        Game::render(std::min(deltaTime, .1));

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

        prevTime = currTime;

    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           !glfwWindowShouldClose(window));

    Game::end();

    // glfwTerminate(); // todo: causes seg fault. ShaderProgram calls openGL to delete program AFTER glfw has terminated.
    return 0;
}
