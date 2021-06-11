
#include <iostream>
#include "cube_map.h"
#include "texture.h"
#include "3d/mesh.h"
#include "../utils/gu_error.h"
#include "../../external/stb/stb_image.h"
#include "../gu/game_utils.h"

CubeMap CubeMap::fromDDSFiles(const std::vector<std::string> &paths)
{
    if (paths.size() != 6)
        throw gu_err("CubeMap::fromDDSFiles needs 6 DDS files");

    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);

    std::vector<DDSData> datas;
    datas.reserve(6);
    for (auto &path : paths)
        datas.emplace_back(path.c_str());

    DDSData &d = datas[0];

    GLsizei width = d.width, height = d.height;

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    GLuint blockSize = (d.format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;

    GLuint size = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;

    int i = 0;
    for (auto &data : datas)
    {
        if (data.width != width || data.height != height || data.format != d.format)
            throw gu_err("CubeMap: DDS files cannot have different dimensions/formats/mipmap-levels");

        glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, d.format, width, height, 0, size, data.buffer);
        i++;
    }
    return CubeMap(id, width, height);
}

CubeMap::CubeMap(GLuint id, GLuint width, GLuint height)
    : id(id), width(width), height(height)
{
    #ifndef PUT_A_SOCK_IN_IT
    std::cout << "CubeMap (" << width << "x" << height << ") id: " << id << " created\n";
    #endif
}

void CubeMap::bind(GLuint unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);
}

CubeMap::~CubeMap()
{
    glDeleteTextures(1, &id);
    #ifndef PUT_A_SOCK_IN_IT
    std::cout << "CubeMap " << id << " destroyed\n";
    #endif
}

namespace equirectangularToCubeMapShader
{

#define MULTILINE(...) #__VA_ARGS__

static const char *vertSource = MULTILINE(

    layout (location = 0) in vec3 aPos;

    out vec3 localPos;

    uniform mat4 projection;
    uniform mat4 view;

    void main()
    {
        localPos = aPos;
        gl_Position = projection * view * vec4(localPos, 1.0);
    }

);

static const char *fragSource = MULTILINE(

    precision mediump float;

    out vec4 fragColor;

    in vec3 localPos;

    uniform sampler2D equirectangularMap;

    const vec2 invAtan = vec2(0.1591, 0.3183);

    vec2 sampleSphericalMap(vec3 v)
    {
        vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
        uv *= invAtan;
        uv += 0.5;
        return uv;
    }

    void main()
    {
        vec2 uv = sampleSphericalMap(normalize(localPos)); // normalize
        vec3 color = texture(equirectangularMap, uv).rgb;
        fragColor = vec4(color, 1.0);
    }

);

}

CubeMap CubeMap::fromHDRFile(const char *path, unsigned int resolution)
{
    Texture eqr = equirectangularFromHDRFile(path);

    ShaderProgram eqrToCubeShader("equirectangular map to cubemap shader", equirectangularToCubeMapShader::vertSource, equirectangularToCubeMapShader::fragSource);
    eqrToCubeShader.use();

    unsigned int captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, resolution, resolution);
    // todo: depth buffer not needed if culling is inverted
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, captureRBO);

    unsigned int envCubemap;
    glGenTextures(1, &envCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        // note that we store each face with 16 bit floating point values
        #ifdef EMSCRIPTEN   // using rgbA instead of rgb fixes WebGL error: "Framebuffer not complete. (status: 0x8cd6) COLOR_ATTACHMENT0: Attachment has an effective format of RGB16F, which is not renderable."
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA16F,
                     resolution, resolution, 0, GL_RGBA, GL_FLOAT, NULL);
        #else
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
                     resolution, resolution, 0, GL_RGB, GL_FLOAT, NULL);
        #endif
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // enable pre-filter mipmap sampling (combatting visible dots artifact)
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    mat4 captureProjection = perspective(radians(90.f), 1.f,
                                                   .1f, 10.f);
    mat4 captureViews[] =
    {
        lookAt(vec3(0.0f, 0.0f, 0.0f), vec3( 1.0f, 0.0f, 0.0f),
                    vec3(0.0f, -1.0f, 0.0f)),
        lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f),
                    vec3(0.0f, -1.0f, 0.0f)),
        lookAt(vec3(0.0f, 0.0f, 0.0f), vec3( 0.0f, 1.0f, 0.0f),
                    vec3(0.0f, 0.0f, 1.0f)),
        lookAt(vec3(0.0f, 0.0f, 0.0f), vec3( 0.0f, -1.0f, 0.0f),
                    vec3(0.0f, 0.0f, -1.0f)),
        lookAt(vec3(0.0f, 0.0f, 0.0f), vec3( 0.0f, 0.0f, 1.0f),
                    vec3(0.0f, -1.0f, 0.0f)),
        lookAt(vec3(0.0f, 0.0f, 0.0f), vec3( 0.0f, 0.0f, -1.0f),
                    vec3(0.0f, -1.0f, 0.0f))
    };
    // convert HDR equirectangular environment map to cubemap equivalent
    eqrToCubeShader.use();
    glUniformMatrix4fv(eqrToCubeShader.location("projection"), 1, GL_FALSE, &captureProjection[0][0]);
    eqr.bind(0, eqrToCubeShader, "equirectangularMap");
    glViewport(0, 0, resolution, resolution); // don’t forget to configure the viewport
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glUniformMatrix4fv(eqrToCubeShader.location("view"), 1, GL_FALSE, &captureViews[i][0][0]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Mesh::getCube()->render();
    }

    // TODO: destroy framebuffer and attachments
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, gu::width, gu::height);


    return CubeMap(envCubemap, resolution, resolution);
}

Texture CubeMap::equirectangularFromHDRFile(const char *path)
{
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    float *data = stbi_loadf(path, &width, &height, &nrComponents, 0);
    stbi_set_flip_vertically_on_load(false);
    unsigned int hdrTexture;
    if (data)
    {
        glGenTextures(1, &hdrTexture);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB,
                     GL_FLOAT, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    else
    {
        throw gu_err("Failed to load HDR file.");
    }
    return Texture(hdrTexture, width, height);
}
