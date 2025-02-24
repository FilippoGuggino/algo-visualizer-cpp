#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define GL_EXT_PROTOTYPES
#define EGS_EGLEXT_PROTOTYPES
#else
#include "glad/glad.h"
#endif
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void main_loop(void* ctx)
{
    GLFWwindow* window = (GLFWwindow*)ctx;

    glClearColor(0.7f, 0.9f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window);
    glfwPollEvents();
}

int main(void)
{
    emscripten_log(EM_LOG_CONSOLE, "main");

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
