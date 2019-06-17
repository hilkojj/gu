#include <iostream>

#include "../gl_includes.h"
#include "../graphics/3d/vert_attributes.h"
#include "../gu/screen.h"
#include "../graphics/shader_program.h"
#include "../graphics/3d/perspective_camera.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

class WebAssemblyScreen : public Screen
{

  public:

    double time = 0;

    WebAssemblyScreen() 
    {
    }

    void render(double deltaTime)
    {
        time += deltaTime;
        glClearColor(fmod(time, 1), .3 * (1 - fmod(time * .5, 1)), .7, 1);
        glClear(GL_COLOR_BUFFER_BIT);

    }

};
