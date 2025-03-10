#include <iostream>
#include <vector>
#include <cmath>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define GL_EXT_PROTOTYPES
#define EGS_EGLEXT_PROTOTYPES
#else
#include "glad/glad.h"
#endif
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
            std::cout << "mouse button " << std::endl;
        } else if (action == GLFW_RELEASE) {
            isDragging = false;
            lastRotation = currentRotation;
        }
    }
}

void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (isDragging) {
        std::cout << "move mouse" << std::endl;
        endVec = screenToArcball(xpos, ypos);
        glm::vec3 axis = glm::cross(startVec, endVec);
        float angle = acos(glm::dot(startVec, endVec));

        if (glm::length(axis) > 0.0001f) {
            axis = glm::normalize(axis);
            glm::quat deltaRotation = glm::angleAxis(angle, axis);
            currentRotation = deltaRotation * lastRotation;
        }
    }
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f + yoffset * 0.1f));
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

unsigned int shaderProgram;

void draw_arrow()
{
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f
    };

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glUseProgram(shaderProgram);

    glDrawArrays(GL_TRIANGLES, 0, 3);
}

// void drawCylinder(float radius, float height, int slices, int stacks)
// {
//     glUseProgram(shaderProgram);

//     // Draw the side of the cylinder (lateral surface)
//     float angleStep = 2 * M_PI / slices; // Step in angle for each slice
//     for (int i = 0; i < slices; ++i) {
//         float angle1 = i * angleStep;
//         float angle2 = (i + 1) * angleStep;

//         glBegin(GL_QUAD_STRIP); // Draw a quad strip between the two angle positions
//         for (int j = 0; j <= stacks; ++j) {
//             float z = j * height / stacks - height / 2.0f; // Calculate the Z coordinate for each stack
//             float x1 = radius * cos(angle1);
//             float y1 = radius * sin(angle1);
//             float x2 = radius * cos(angle2);
//             float y2 = radius * sin(angle2);

//             glVertex3f(x1, y1, z); // Vertex for the current angle
//             glVertex3f(x2, y2, z); // Vertex for the next angle
//         }
//         glEnd();
//     }

//     // Draw the top cap of the cylinder (a circle)
//     glBegin(GL_TRIANGLE_FAN);
//     glVertex3f(0.0f, 0.0f, height / 2.0f); // Center of the top cap
//     for (int i = 0; i <= slices; ++i) {
//         float angle = i * angleStep;
//         float x = radius * cos(angle);
//         float y = radius * sin(angle);
//         glVertex3f(x, y, height / 2.0f);
//     }
//     glEnd();

//     // Draw the bottom cap of the cylinder (a circle)
//     glBegin(GL_TRIANGLE_FAN);
//     glVertex3f(0.0f, 0.0f, -height / 2.0f); // Center of the bottom cap
//     for (int i = 0; i <= slices; ++i) {
//         float angle = i * angleStep;
//         float x = radius * cos(angle);
//         float y = radius * sin(angle);
//         glVertex3f(x, y, -height / 2.0f);
//     }
//     glEnd();
// }

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

    // glClearColor(0.7f, 0.9f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgram);

    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -3));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIN_WIDTH / WIN_HEIGHT, 0.1f, 100.0f);
    glm::mat4 model = glm::mat4_cast(currentRotation);

    GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
    GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
    GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);

    print_mat4(model);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size() / 3);

    // draw_arrow();
    // drawCylinder(5, 5, 30, 30);

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
        const char* vertexShaderSource = R"(
            #version 300 es
            // precision mediump float;
            layout (location = 0) in vec3 aPos;
            uniform mat4 model;
            // uniform mat4 view;
            // uniform mat4 projection;
            void main() {
                gl_Position = model * vec4(aPos, 1.0);
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
        const char* fragmentShaderSource = R"(
            #version 300 es
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
        std::cout << "compiled: " << isCompiled << std::endl;
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
    // emscripten_log(EM_LOG_CONSOLE, "main");

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    const uint kWinWidth = 800;
    const uint kWinHeight = 800;

    GLFWwindow* window = glfwCreateWindow(kWinWidth, kWinHeight, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

#ifdef __EMSCRIPTEN__
#else
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
#endif

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

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(main_loop, (void*)window, 0, true);
#else
    while (!glfwWindowShouldClose(window)) {
        // main_loop((void*)window);
        glClearColor(0.7f, 0.9f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -3));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIN_WIDTH / WIN_HEIGHT, 0.1f, 100.0f);
        glm::mat4 model = glm::mat4_cast(currentRotation);

        GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
        GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);

        print_mat4(model);

        // glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size() / 3);
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0);

        // draw_arrow();
        // drawCylinder(5, 5, 30, 30);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
#endif

    glfwTerminate();

    return 0;
}
