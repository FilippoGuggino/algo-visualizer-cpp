#include <expected>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include "canvas.h"
#include "geometry.h"
#include "log.h"
#include "shaders_data.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

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

static std::expected<unsigned int, std::string> compile_shader(const char* data, int shader_type)
{
    unsigned int shader;

    shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &data, NULL);
    glCompileShader(shader);

    GLint isCompiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> error_log(maxLength);
        glGetShaderInfoLog(shader, maxLength, &maxLength, &error_log[0]);

        glDeleteShader(shader);
        return std::unexpected(error_log.data());
    }

    return shader;
}

Canvas::Canvas(GLFWwindow* window)
    : m_window(window)
    , m_rotation_matrix(glm::identity<glm::mat4>())
    , m_scale_matrix(glm::identity<glm::mat4>())
    , m_pan_matrix(glm::identity<glm::mat4>())
    , m_static_view_matrix(glm::lookAt(glm::vec3(0, 0, 100), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)))
{
    glfwGetFramebufferSize(window, &m_width, &m_height);
    m_projection = glm::ortho(-m_width / 2.0, m_width / 2.0, m_height / 2.0, -m_height / 2.0, 1.0, 1000.0);

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);

    std::expected<unsigned int, std::string> vertex_shader;
    if (vertex_shader = compile_shader(default_vert, GL_VERTEX_SHADER); !vertex_shader.has_value()) {
        log_console("Could not compile vertex shader, " + vertex_shader.error());
    }

    std::expected<unsigned int, std::string> fragment_shader;
    if (fragment_shader = compile_shader(default_frag, GL_FRAGMENT_SHADER); !fragment_shader.has_value()) {
        log_console("Could not compile fragment shader, " + vertex_shader.error());
    }

    m_shader = glCreateProgram();

    glAttachShader(m_shader, *vertex_shader);
    glAttachShader(m_shader, *fragment_shader);
    glLinkProgram(m_shader);

    glDeleteShader(*vertex_shader);
    glDeleteShader(*fragment_shader);

    // OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEBUG_OUTPUT);
}

glm::mat4 Canvas::view_matrix()
{
    return m_static_view_matrix * m_pan_matrix * m_rotation_matrix * m_scale_matrix;
}

void Canvas::render()
{
    glClearColor(0.7f, 0.9f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_shader);

    GLuint viewLoc = glGetUniformLocation(m_shader, "view");
    m_view = view_matrix();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &m_view[0][0]);

    GLuint projectionLoc = glGetUniformLocation(m_shader, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &m_projection[0][0]);

    for (auto& g : m_geometries) {
        if (g) {
            g->draw(m_shader);
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
    m_projection = glm::ortho(-m_width / 2.0, m_width / 2.0, m_height / 2.0, -m_height / 2.0, 1.0, 1000.0);
}

void Canvas::on_mouse_button_callback(int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            m_is_dragging_rotation = true;
            double xpos, ypos;
            glfwGetCursorPos(m_window, &xpos, &ypos);
            m_start_vec_rotation = screenToArcball(xpos, ypos, m_width, m_height);
        } else if (action == GLFW_RELEASE) {
            m_is_dragging_rotation = false;
            m_last_rotation = m_current_rotation;
        }
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            m_is_panning = true;
            double xpos, ypos;
            glfwGetCursorPos(m_window, &xpos, &ypos);
            m_start_pan_world_position = glm::unProject(glm::vec3(xpos, m_height - ypos, 0.0f), m_static_view_matrix, m_projection, glm::vec4(0.0f, 0.0f, m_width, m_height));
        } else if (action == GLFW_RELEASE) {
            m_is_panning = false;
            // "Commit" the translation by saving it
            m_last_pan_translation = glm::vec3(m_pan_matrix[3]);
        }
    }
}

void Canvas::on_cursor_position_callback(double xpos, double ypos)
{
    if (m_is_dragging_rotation) {
        glm::vec3 current_vec_rotation = screenToArcball(xpos, ypos, m_width, m_height);
        glm::vec3 axis = glm::cross(m_start_vec_rotation, current_vec_rotation);
        float angle = acos(glm::dot(m_start_vec_rotation, current_vec_rotation));

        if (glm::length(axis) > 0.0001f) {
            axis = glm::normalize(axis);
            glm::quat deltaRotation = glm::angleAxis(angle, axis);
            m_current_rotation = deltaRotation * m_last_rotation;
            m_rotation_matrix = glm::mat4_cast(m_current_rotation);
        }
    } else if (m_is_panning) {
        // Find the current world position of the cursor on the Z=0 plane
        glm::vec3 current_world_position = glm::unProject(glm::vec3(xpos, m_height - ypos, 0.0f), m_static_view_matrix, m_projection, glm::vec4(0.0f, 0.0f, m_width, m_height));
        // Calculate the displacement from the start of the pan
        glm::vec3 delta = current_world_position - m_start_pan_world_position;

        // Update the pan matrix by adding the delta to the last committed translation
        m_pan_matrix = glm::translate(glm::mat4(1.0f), m_last_pan_translation + delta);
    }
}

void Canvas::on_scroll_callback(double xoffset, double yoffset)
{
    m_scale_matrix *= glm::scale(glm::mat4(1.0f), glm::vec3(1.0f + yoffset * 0.1f));
}
