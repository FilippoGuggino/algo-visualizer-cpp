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
    , m_last_pan_matrix(glm::identity<glm::mat4>())
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

glm::dmat4 Canvas::view_matrix()
{
    return m_static_view_matrix * m_rotation_matrix * m_scale_matrix * m_pan_matrix;
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
    glm::mat4 proj = m_projection;
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &proj[0][0]);

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
            // --- FIX ---
            // Instead of calculating a world position, we now save the cursor
            // position and the current pan matrix. This makes the panning
            // operation cumulative and prevents conflicts with the zoom's panning.
            m_start_pan_cursor = glm::dvec2(xpos, ypos);
            m_last_pan_matrix = m_pan_matrix;
        } else if (action == GLFW_RELEASE) {
            m_is_panning = false;
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
        glm::dvec4 viewport(0.0f, 0.0f, m_width, m_height);

        // We use the static_view_matrix for unprojection to get a consistent panning speed
        // regardless of the current rotation or scale.
        glm::dvec3 start_world = glm::unProject(glm::dvec3(m_start_pan_cursor.x, m_height - m_start_pan_cursor.y, 0.0f), m_static_view_matrix, m_projection, viewport);
        glm::dvec3 current_world = glm::unProject(glm::dvec3(xpos, m_height - ypos, 0.0f), m_static_view_matrix, m_projection, viewport);

        glm::dvec3 delta = current_world - start_world;

        // Apply the translation delta to the matrix state from the start of the pan.
        m_pan_matrix = glm::translate(glm::dmat4(1.0f), delta) * m_last_pan_matrix;
    }
}

// void Canvas::on_scroll_callback(double xoffset, double yoffset)
// {
//     glm::vec4 viewport(0.0f, 0.0f, m_width, m_height);

//     double xpos, ypos;
//     glfwGetCursorPos(m_window, &xpos, &ypos);

//     glm::dvec3 near_point = glm::unProject(glm::vec3(xpos, m_height - ypos, 0.01f), view_matrix(), m_projection, viewport);
//     glm::dvec3 far_point = glm::unProject(glm::vec3(xpos, m_height - ypos, 0.99f), view_matrix(), m_projection, viewport);
//     glm::dvec3 ray_dir = glm::normalize(far_point - near_point);

//     double t_before = -near_point.z / ray_dir.z;
//     if (t_before < 0) {
//         t_before = 0;
//     }
//     if (t_before > 1) {
//         t_before = 1;
//     }

//     glm::dvec3 cursor_world_before = near_point + t_before * ray_dir;

//     m_scale_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f + yoffset * 0.1f)) * m_scale_matrix;

//     glm::dvec3 near_point_after = glm::unProject(glm::vec3(xpos, m_height - ypos, 0.01f), view_matrix(), m_projection, viewport);
//     glm::dvec3 far_point_after = glm::unProject(glm::vec3(xpos, m_height - ypos, 0.99f), view_matrix(), m_projection, viewport);
//     glm::dvec3 ray_dir_after = glm::normalize(far_point_after - near_point_after);

//     double t_after = -near_point_after.z / ray_dir_after.z;
//     if (t_after < 0) {
//         t_after = 0;
//     }
//     if (t_after > 1) {
//         t_after = 1;
//     }

//     glm::dvec3 cursor_world_after = near_point_after + t_after * ray_dir_after;

//     glm::dvec3 delta = (cursor_world_before - cursor_world_after);
//     m_pan_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(delta)) * m_pan_matrix;
// }

void Canvas::on_scroll_callback(double xoffset, double yoffset)
{
    // This function implements zooming towards the mouse cursor. The logic is:
    // 1. Find the world-space position under the cursor BEFORE zooming.
    // 2. Apply the zoom (scaling transformation).
    // 3. Find the new world-space position under the same cursor position AFTER zooming.
    // 4. Calculate the difference (delta) between the before and after positions.
    // 5. Apply a translation (pan) equal to this delta to counteract the shift.

    glm::vec4 viewport(0.0f, 0.0f, m_width, m_height);
    double xpos, ypos;
    glfwGetCursorPos(m_window, &xpos, &ypos);

    // --- 1. Find world position before zoom ---
    glm::dvec3 cursor_world_before;
    {
        // Get the combined view matrix *before* the new scale is applied
        glm::dmat4 view_before = view_matrix();

        // Unproject the cursor's screen position to get a ray in world space
        glm::dvec3 near_point = glm::unProject(glm::dvec3(xpos, m_height - ypos, 0.0f), view_before, m_projection, viewport);
        glm::dvec3 far_point = glm::unProject(glm::dvec3(xpos, m_height - ypos, 1.0f), view_before, m_projection, viewport);
        glm::dvec3 ray_dir = glm::normalize(far_point - near_point);

        // Find where this ray intersects the Z=0 plane.
        // Ray equation: P(t) = near_point + t * ray_dir
        // We want P(t).z = 0, so: near_point.z + t * ray_dir.z = 0
        // Which gives: t = -near_point.z / ray_dir.z
        if (std::abs(ray_dir.z) > 1e-6) { // Avoid division by zero
            double t = -near_point.z / ray_dir.z;
            cursor_world_before = near_point + t * ray_dir;
        } else {
            return; // Ray is parallel to the plane, can't determine a point to zoom to.
        }
    }

    // --- 2. Apply scaling ---
    float scale_factor = 1.0f + yoffset * 0.1f;
    m_scale_matrix = glm::scale(glm::dmat4(1.0f), glm::dvec3(scale_factor)) * m_scale_matrix;

    // --- 3. Find world position after zoom ---
    glm::dvec3 cursor_world_after;
    {
        // Get the combined view matrix *after* the new scale is applied
        glm::dmat4 view_after = view_matrix();
        glm::dvec3 near_point = glm::unProject(glm::dvec3(xpos, m_height - ypos, 0.0f), view_after, m_projection, viewport);
        glm::dvec3 far_point = glm::unProject(glm::dvec3(xpos, m_height - ypos, 1.0f), view_after, m_projection, viewport);
        glm::dvec3 ray_dir = glm::normalize(far_point - near_point);

        if (std::abs(ray_dir.z) > 1e-6) {
            double t = -near_point.z / ray_dir.z;
            cursor_world_after = near_point + t * ray_dir;
        } else {
            return;
        }
    }

    // --- 4. Calculate the correction delta ---
    glm::dvec3 delta = cursor_world_after - cursor_world_before;

    // --- 5. Apply the correction pan ---
    // Pre-multiply the pan matrix with a translation for the delta.
    // This shifts the entire scene to keep the point under the cursor stationary.
    m_pan_matrix = glm::translate(glm::dmat4(1.0f), delta) * m_pan_matrix;

    glm::dvec3 cursor_world_after_pan;
    {
        // Get the combined view matrix *after* the new scale is applied
        glm::dmat4 view_after = view_matrix();
        glm::dvec3 near_point = glm::unProject(glm::dvec3(xpos, m_height - ypos, 0.0f), view_after, m_projection, viewport);
        glm::dvec3 far_point = glm::unProject(glm::dvec3(xpos, m_height - ypos, 1.0f), view_after, m_projection, viewport);
        glm::dvec3 ray_dir = glm::normalize(far_point - near_point);

        if (std::abs(ray_dir.z) > 1e-6) {
            double t = -near_point.z / ray_dir.z;
            cursor_world_after_pan = near_point + t * ray_dir;
        } else {
            return;
        }
    }
    std::cout << "pan_matrix: " << glm::to_string(m_pan_matrix) << std::endl;

    // --- Optional Debug Output ---
    // Uncomment the line below to see the calculated values in your console.
    // This is a great way to debug and understand the process.
    printf("World Before: (%.2f, %.2f, %.2f) | World After Zoom: (%.2f, %.2f, %.2f) | World After Zoom+Pan: (%.2f, %.2f, %.2f) | Delta: (%.2f, %.2f, %.2f)\n",
        cursor_world_before.x, cursor_world_before.y, cursor_world_before.z,
        cursor_world_after.x, cursor_world_after.y, cursor_world_after.z,
        cursor_world_after_pan.x, cursor_world_after_pan.y, cursor_world_after_pan.z,
        delta.x, delta.y, delta.z);
}
