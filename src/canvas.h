#ifndef CANVAS_H
#define CANVAS_H

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class GLFWwindow;
class Geometry;

class Canvas {
public:
    Canvas(GLFWwindow* window);
    void add_geometry(Geometry* geometry);
    glm::mat4 view_matrix();
    void render();

    void on_framebuffer_resize(int newWidth, int newHeight);
    void on_mouse_button_callback(int button, int action, int mods);
    void on_cursor_position_callback(double xpos, double ypos);
    void on_scroll_callback(double xoffset, double yoffset);

private:
    GLFWwindow* m_window;

    unsigned int m_shader;

    std::vector<Geometry*> m_geometries;

    int m_width;
    int m_height;

    glm::mat4 modelMatrix = glm::mat4(1.0f);

    // Arcball variables
    bool m_is_dragging_rotation = false;
    glm::vec3 m_start_vec_rotation;
    glm::quat m_current_rotation = glm::quat(1, 0, 0, 0);
    glm::quat m_last_rotation = glm::quat(1, 0, 0, 0);

    bool m_is_dragging_translation = false;
    glm::vec2 m_start_vec_translation;
    glm::vec2 m_current_translation = glm::zero<glm::vec2>();
    glm::vec2 m_last_translation = glm::zero<glm::vec2>();

    glm::mat4 m_static_view_matrix;
    glm::mat4 m_translation_matrix;
    glm::mat4 m_rotation_matrix;
    glm::mat4 m_scale_matrix;

    glm::mat4 m_view;
    glm::mat4 m_projection;
};

#endif /* CANVAS_H */
