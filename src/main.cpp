#include <iostream>
#include <vector>
#include <cmath>
#include "spdlog/spdlog.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define GL_EXT_PROTOTYPES
#define EGS_EGLEXT_PROTOTYPES
#define log_console(S) emscripten_log(EM_LOG_CONSOLE, S)
#else
// disable log
// TODO redirect with spdlog
#define log_console(S) spdlog::info(S)
#endif

#include "glad/glad.h"
#include <GLFW/glfw3.h>
// #include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

std::vector<float> vertices;

// Window dimensions
const int WIN_WIDTH = 800, WIN_HEIGHT = 600;
glm::mat4 modelMatrix = glm::mat4(1.0f);

// Arcball variables
bool isDragging = false;
glm::vec3 startVec, endVec;
glm::quat currentRotation = glm::quat(1, 0, 0, 0);
glm::quat lastRotation = glm::quat(1, 0, 0, 0);
glm::mat4 rotation_matrix;
glm::mat4 scale_matrix;
glm::vec3 scale_vec;

glm::mat4 view;
glm::mat4 projection;
glm::mat4 model;

// Convert screen coordinates to arcball sphere
glm::vec3 screenToArcball(float x, float y)
{
    glm::vec3 p = glm::vec3(
        (2.0f * x - WIN_WIDTH) / WIN_WIDTH,
        (WIN_HEIGHT - 2.0f * y) / WIN_HEIGHT,
        0.0f);

    float mag = p.x * p.x + p.y * p.y;
    if (mag <= 1.0f) {
        p.z = sqrt(1.0f - mag); // Point is on sphere
    } else {
        p = glm::normalize(p); // Point is on hyperbolic sheet
    }
    return p;
}

// Mouse callbacks
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            isDragging = true;
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            startVec = screenToArcball(xpos, ypos);
        } else if (action == GLFW_RELEASE) {
            isDragging = false;
            lastRotation = currentRotation;
        }
    }
}

void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (isDragging) {
        endVec = screenToArcball(xpos, ypos);
        glm::vec3 axis = glm::cross(startVec, endVec);
        float angle = acos(glm::dot(startVec, endVec));

        if (glm::length(axis) > 0.0001f) {
            axis = glm::normalize(axis);
            glm::quat deltaRotation = glm::angleAxis(angle, axis);
            currentRotation = deltaRotation * lastRotation;
            rotation_matrix = glm::mat4_cast(currentRotation);
        }
    }
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    scale_vec *= glm::vec3(1.0f + yoffset * 0.1f);
    scale_matrix = glm::scale(glm::mat4(1.0f), scale_vec);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

unsigned int shaderProgram;

// Create VAO and VBO
GLuint VAO, VBO;

void print_mat4(glm::mat4 m)
{
    for (size_t i = 0; i < m.length(); i++) {
        for (size_t j = 0; j < m[0].length(); j++) {
            std::cout << m[i][j] << "  ";
        }
        std::cout << std::endl;
    }
}

void main_loop(void* ctx)
{
    GLFWwindow* window = (GLFWwindow*)ctx;

    glClearColor(0.7f, 0.9f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgram);

    view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
    projection = glm::perspective(glm::radians(45.0f), (float)WIN_WIDTH / WIN_HEIGHT, 0.1f, 100.0f);

    GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
    model = rotation_matrix * scale_matrix;
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
    GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
    GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
}

// Cylinder parameters
const int SEGMENTS = 50;
const float RADIUS = 0.2f;
const float HEIGHT = 1.0f;

std::vector<float> generateCylinderVertices()
{
    std::vector<float> vertices;

    for (int i = 0; i <= SEGMENTS; i++) {
        float angle = 2.0f * M_PI * i / SEGMENTS;
        float x = RADIUS * cos(angle);
        float z = RADIUS * sin(angle);

        // Top circle
        vertices.push_back(x);
        vertices.push_back(HEIGHT / 2.0f);
        vertices.push_back(z);

        // Bottom circle
        vertices.push_back(x);
        vertices.push_back(-HEIGHT / 2.0f);
        vertices.push_back(z);
    }
    return vertices;
}

void load_shader()
{
    unsigned int vertexShader;
    {
        const char* vertexShaderSource = R"(#version 300 es
            precision mediump float;
            layout (location = 0) in vec3 aPos;
            uniform mat4 model;
            uniform mat4 view;
            // uniform mat4 projection;
            void main() {
                gl_Position = model * view * vec4(aPos, 1.0);
            })";

        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);

        GLint isCompiled;
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE) {
            GLint maxLength = 0;
            glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> errorLog(maxLength);
            glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &errorLog[0]);

            std::cout << errorLog.data() << std::endl;

            // Provide the infolog in whatever manor you deem best.
            // Exit with failure.
            glDeleteShader(vertexShader); // Don't leak the shader.
            return;
        }
    }

    unsigned int fragmentShader;
    {
        const char* fragmentShaderSource = R"(#version 300 es
            precision mediump float;
            layout(location = 0) out vec4 out_color;
            void main() {
                out_color = vec4(1.0, 1.0, 1.0, 1.0);
            })";

        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);

        GLint isCompiled;
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE) {
            GLint maxLength = 0;
            glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> errorLog(maxLength);
            glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &errorLog[0]);

            std::cout << errorLog.data() << std::endl;

            // Provide the infolog in whatever manor you deem best.
            // Exit with failure.
            glDeleteShader(fragmentShader); // Don't leak the shader.
            return;
        }
    }

    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

int main(void)
{
    log_console("main");

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    const unsigned int kWinWidth = 800;
    const unsigned int kWinHeight = 800;

    GLFWwindow* window = glfwCreateWindow(kWinWidth, kWinHeight, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    load_shader();

    glViewport(0, 0, kWinWidth, kWinHeight);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // Set mouse callbacks
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetScrollCallback(window, scrollCallback);

    // OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEBUG_OUTPUT);

    std::vector<GLfloat> vertices = {
        0.5f, 0.5f, 0.0f, // top right
        0.5f,
        -0.5f,
        0.0f, // bottom right
        -0.5f,
        -0.5f,
        0.0f, // bottom left
        -0.5f,
        0.5f,
        0.0f // top left
    };
    unsigned int indices[] = {
        // note that we start from 0!
        0,
        1,
        3, // first triangle
        1,
        2,
        3 // second triangle
    };

    // vertices = generateCylinderVertices();

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    model = glm::identity<glm::mat4>();
    rotation_matrix = glm::identity<glm::mat4>();
    scale_matrix = glm::identity<glm::mat4>();
    scale_vec = glm::one<glm::vec3>();

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(main_loop, (void*)window, 0, true);
#else
    while (!glfwWindowShouldClose(window)) {
        main_loop((void*)window);
    }
#endif

    glfwTerminate();

    return 0;
}
