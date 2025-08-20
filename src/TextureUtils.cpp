#include "TextureUtils.h"
#include <unordered_map>

// GLenums used for reading from JSON don't work if json data is incorrect
std::unordered_map<std::string, GLenum> glEnumMap = {
    {"GL_RGBA", GL_RGBA},
    {"GL_DEPTH_STENCIL", GL_DEPTH_STENCIL},
    {"GL_DEPTH_COMPONENT", GL_DEPTH_COMPONENT},
    {"GL_RGB", GL_RGB},
    {"GL_RG", GL_RG},
    {"GL_RED", GL_RED},
    {"GL_RGBA16F", GL_RGBA16F},
    {"GL_R32I", GL_R32I},
    {"GL_RED_INTEGER", GL_RED_INTEGER},
    {"GL_INT", GL_INT},
    {"GL_FLOAT", GL_FLOAT},
    {"GL_UNSIGNED_BYTE", GL_UNSIGNED_BYTE},
    {"GL_COLOR_ATTACHMENT0", GL_COLOR_ATTACHMENT0},
    {"GL_COLOR_ATTACHMENT1", GL_COLOR_ATTACHMENT1},
    {"GL_COLOR_ATTACHMENT2", GL_COLOR_ATTACHMENT2},
    {"GL_COLOR_ATTACHMENT3", GL_COLOR_ATTACHMENT3},
    {"GL_COLOR_ATTACHMENT4", GL_COLOR_ATTACHMENT4},
    {"GL_DEPTH_ATTACHMENT", GL_DEPTH_ATTACHMENT},
    {"GL_DEPTH_COMPONENT", GL_DEPTH_COMPONENT},
    {"GL_DEPTH_COMPONENT24", GL_DEPTH_COMPONENT24},
};

// Reading the GLenums properly
GLenum getGLEnumFromString(const std::string& str) {
    auto it = glEnumMap.find(str);
    if (it != glEnumMap.end())
        return it->second;
    else
        throw std::runtime_error("Unknown GLenum string: " + str);
        return 0;
};


