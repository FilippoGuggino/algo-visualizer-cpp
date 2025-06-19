
#include "canvas.h"
#include "geometry.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "log.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

void render(void* ctx)
{
    auto canvas = static_cast<Canvas*>(ctx);
    canvas->render();
}

int main(void)
{
    log_console("Starting application");

    unsigned int width = 800;
    unsigned int height = 800;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(width, height, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        spdlog::error("Failed to create GLFW window");
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        spdlog::error("Failed to initialize GLAD");
    }

    glViewport(0, 0, width, height);

    Canvas canvas(window);

    Cylinder c(50.0, 20, 30);
    canvas.add_geometry(&c);

    // Rectangle rec(1.0, 1.0);
    // canvas.add_geometry(&rec);

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(render, &canvas, 0, true);
#else
    while (!glfwWindowShouldClose(window)) {
        render(&canvas);
    }
#endif

    glfwTerminate();

    return 0;
}
