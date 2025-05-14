#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include "canvas.h"
#include "geometry.h"

// Convert screen coordinates to arcball sphere
static glm::vec3 screenToArcball(float x, float y, unsigned int width, unsigned int height)
{
    glm::vec3 p = glm::vec3(
        (2.0f * x - width) / width,
        (height - 2.0f * y) / height,
        0.0f);

    float mag = p.x * p.x + p.y * p.y;
    if (mag <= 1.0f) {
        p.z = sqrt(1.0f - mag); // Point is on sphere
    } else {
        p = glm::normalize(p); // Point is on hyperbolic sheet
    }
    return p;
}

static void load_shader(unsigned int& shader)
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

            // std::cout << errorLog.data() << std::endl;

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

            // std::cout << errorLog.data() << std::endl;

            // Provide the infolog in whatever manor you deem best.
            // Exit with failure.
            glDeleteShader(fragmentShader); // Don't leak the shader.
            return;
        }
    }

    shader = glCreateProgram();

    glAttachShader(shader, vertexShader);
    glAttachShader(shader, fragmentShader);
    glLinkProgram(shader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    auto canvas = static_cast<Canvas*>(glfwGetWindowUserPointer(window));
    if (canvas) {
        canvas->on_framebuffer_resize(width, height);
    }
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    auto canvas = static_cast<Canvas*>(glfwGetWindowUserPointer(window));
    if (canvas) {
        canvas->on_mouse_button_callback(button, action, mods);
    }
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    auto canvas = static_cast<Canvas*>(glfwGetWindowUserPointer(window));
    if (canvas) {
        canvas->on_cursor_position_callback(xpos, ypos);
    }
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    auto canvas = static_cast<Canvas*>(glfwGetWindowUserPointer(window));
    if (canvas) {
        canvas->on_scroll_callback(xoffset, yoffset);
    }
}

Canvas::Canvas(GLFWwindow* window)
    : m_window(window)
    , m_model(glm::identity<glm::mat4>())
    , m_rotation_matrix(glm::identity<glm::mat4>())
    , m_scale_matrix(glm::identity<glm::mat4>())
{
    glfwGetFramebufferSize(window, &m_width, &m_height);

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);

    load_shader(m_shader);

    // OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEBUG_OUTPUT);
}

void Canvas::render()
{
    glClearColor(0.7f, 0.9f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_shader);

    m_view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
    m_projection = glm::perspective(glm::radians(45.0f), (float)m_width / m_height, 0.1f, 100.0f);

    GLuint modelLoc = glGetUniformLocation(m_shader, "model");
    m_model = m_rotation_matrix * m_scale_matrix;
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &m_model[0][0]);
    GLuint viewLoc = glGetUniformLocation(m_shader, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &m_view[0][0]);
    GLuint projectionLoc = glGetUniformLocation(m_shader, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &m_projection[0][0]);

    for (auto& g : m_geometries) {
        if (g) {
            g->draw();
        }
    }

    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

void Canvas::add_geometry(Geometry* geometry)
{
    m_geometries.push_back(geometry);
}

void Canvas::on_framebuffer_resize(int newWidth, int newHeight)
{
    glViewport(0, 0, newWidth, newHeight);
    m_height = newHeight;
    m_width = newWidth;
}

void Canvas::on_mouse_button_callback(int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            isDragging = true;
            double xpos, ypos;
            glfwGetCursorPos(m_window, &xpos, &ypos);
            m_start_vec = screenToArcball(xpos, ypos, m_width, m_height);
        } else if (action == GLFW_RELEASE) {
            isDragging = false;
            m_last_rotation = m_current_rotation;
        }
    }
}

void Canvas::on_cursor_position_callback(double xpos, double ypos)
{
    if (isDragging) {
        m_end_vec = screenToArcball(xpos, ypos, m_width, m_height);
        glm::vec3 axis = glm::cross(m_start_vec, m_end_vec);
        float angle = acos(glm::dot(m_start_vec, m_end_vec));

        if (glm::length(axis) > 0.0001f) {
            axis = glm::normalize(axis);
            glm::quat deltaRotation = glm::angleAxis(angle, axis);
            m_current_rotation = deltaRotation * m_last_rotation;
            m_rotation_matrix = glm::mat4_cast(m_current_rotation);
        }
    }
}

void Canvas::on_scroll_callback(double xoffset, double yoffset)
{
    m_scale_matrix *= glm::scale(glm::mat4(1.0f), glm::vec3(1.0f + yoffset * 0.1f));
}
