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

std::vector<float> vertices;

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

void main_loop(void* ctx)
{
    GLFWwindow* window = (GLFWwindow*)ctx;

    glClearColor(0.7f, 0.9f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
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
            #version 330 core
            layout (location = 0) in vec3 aPos;
            void main() {
                gl_Position = vec4(aPos, 1.0);
            }
            )";
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);
    }
    unsigned int fragmentShader;
    {
        const char* fragmentShaderSource = R"(
            #version 330 core
            out vec4 FragColor;
            void main() {
                FragColor = vec4(1.0, 0.5, 0.2, 1.0);
            }
            )";

        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);
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

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
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

    glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    vertices = generateCylinderVertices();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

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
