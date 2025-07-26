#include "ShaderUtils.h"
#include <SDL2/SDL.h>


int ScreenHeight = 512;
int ScreenWidth = 512;

unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;

unsigned int quadVAO = 0;
unsigned int quadVBO = 0;

void CheckSDLError(const std::string& message) 
{
    const char* error = SDL_GetError();
    if (*error != '\0') 
    {
        std::cerr << "SDL Error (" << message << "): " << error << std::endl;
        SDL_ClearError();
        exit(1);
    }
}

void CheckGLError(const std::string& message) 
{
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) 
    {
        std::cerr << "OpenGL Error (" << message << "): " << error << std::endl;
        exit(1);
    }
}



void renderQuad()
{
    if (quadVAO == 0)
    {
        float screenQuadVerticies[] = 
        {
            1.0f,  1.0f, 0.0f,   1.0f, 1.0f,
            1.0f, -1.0f, 0.0f,   1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f,   0.0f, 1.0f,
            1.0f, -1.0f, 0.0f,   1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,
            -1.0f,  1.0f, 0.0f,   0.0f, 1.0f
        };

        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER,sizeof(screenQuadVerticies),&screenQuadVerticies,GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3,GL_FLOAT, GL_FALSE, 5 * sizeof(float),(void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2,GL_FLOAT, GL_FALSE, 5 * sizeof(float),(void*)(3*sizeof(float)));
        CheckGLError("Make Quad");
    }
    
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    CheckGLError("Render Quad");
}



void renderCube()
{
    if (cubeVAO == 0)
    {
        float cubeVerticies[] = 
            {
                -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
                1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
                1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
                -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
                // front face
                -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
                1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
                1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
                -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
                -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                // left face
                -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
                -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
                -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
                -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
                -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
                -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
                // right face
                1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
                1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
                1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
                1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
                // bottom face
                -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
                1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
                1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
                1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
                -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
                -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
                // top face
                -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
                1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left     
            };
            glGenVertexArrays(1, &cubeVAO);
            glGenBuffers(1, &cubeVBO);
            // fill buffer
            glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerticies), &cubeVerticies, GL_STATIC_DRAW);
            // link vertex attributes
            glBindVertexArray(cubeVAO);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
            std::cout << "CubeMade" << std::endl;
            CheckGLError("Make Cube");
    }
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    CheckGLError("Render Cube");
    
}


std::string readFile(const std::string& path) {
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint loadComputeShader(const std::string& path) {
    GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
    std::string src = readFile(path);
    const char* cstr = src.c_str();
    glShaderSource(shader, 1, &cstr, NULL);
    glCompileShader(shader);
    if (glGetError() != GL_NO_ERROR) {
        std::cerr << "Error compiling compute shader: " << path << std::endl;
        return 0;
    }
    GLuint program = glCreateProgram();
    glAttachShader(program, shader);
    glLinkProgram(program);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "Compute Shader Compilation Failed:\n" << infoLog << std::endl;
    }
    return program;
}

GLuint loadShaderProgram(const std::string& inVertPath, const std::string& inFragPath) {
    auto load = [](const std::string& path, GLenum type) {
        GLuint shader = glCreateShader(type);
        std::string src = readFile(path);
        const char* cstr = src.c_str();
        glShaderSource(shader, 1, &cstr, NULL);
        glCompileShader(shader);
        return shader;
    };
    std::string vertPath = (std::string(ASSET_DIR) + inVertPath);
    std::string fragPath = (std::string(ASSET_DIR) + inFragPath);

    GLuint vert = load(vertPath, GL_VERTEX_SHADER);
    GLuint frag = load(fragPath, GL_FRAGMENT_SHADER);
    GLuint program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "Shader Program Linking Failed:\n" << infoLog << std::endl;
        return 0;
    }
    std::cout << "Shader Program Linked Successfully: " << inVertPath << " and " << inFragPath << std::endl;
    return program;
}

void setInt (GLuint program, const std::string& name, int value)
{
    GLuint location = glGetUniformLocation(program,name.c_str());
    glUniform1i(location,value);
}

void setFloat(GLuint program, const std::string& name, float value)
{
    GLuint location = glGetUniformLocation(program, name.c_str());
    glUniform1f(location, value);
}

void setMat4(GLuint program, const std::string& name, const glm::mat4 &value) 
{
    GLint location = glGetUniformLocation(program, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
}

void setVec3(GLuint program, const std::string& name, const glm::vec3 &value)
{
    GLint location = glGetUniformLocation(program, name.c_str());
    glUniform3fv(location,1,&value[0]);
}