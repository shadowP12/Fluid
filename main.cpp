#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include "Fluid2D.h"

static Fluid2D* gFluid2D = nullptr;
bool gMouseButtonHeld[3] = { false, false, false };
bool gMouseButtonDown[3] = { false, false, false };
bool gMouseButtonUp[3] = { false, false, false };
static glm::vec2 gMousePosition;
static glm::vec2 gMousePreviouPosition;
static int gWidth = 512;
static int gHeight = 512;

void cursorPosCallback(GLFWwindow * window, double posX, double posY);
void mouseButtonCallback(GLFWwindow * window, int button, int action, int mods);

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    GLFWwindow* window = nullptr;
    window = glfwCreateWindow(gWidth, gHeight, "Fluid", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    gFluid2D = new Fluid2D(gWidth);
    while (!glfwWindowShouldClose(window)) {
        glViewport(0, 0, gWidth, gHeight);
        glClearColor(0.3f, 0.3f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        gFluid2D->update(0.125);
        gFluid2D->render();
        if(gMouseButtonDown[0]) {
            gMousePreviouPosition = gMousePosition;
        }
        if(gMouseButtonHeld[0]) {
            glm::vec2 offset = gMousePosition - gMousePreviouPosition;
            if (glm::length(offset) > 1.0f) {
                float x0, x1, y0, y1, dx, dy;
                x0 = gMousePosition.x / gWidth;
                y0 = 1.0f - gMousePosition.y / gHeight;
                x1 = gMousePreviouPosition.x / gWidth;
                y1 = 1.0f - gMousePreviouPosition.y / gHeight;
                dx = x0 - x1;
                dy = y0 - y1;
                gFluid2D->splat(x0, y0, dx, dy);
            }
            gMousePreviouPosition = gMousePosition;
        }
        memset(gMouseButtonUp, 0, sizeof(gMouseButtonUp));
        memset(gMouseButtonDown, 0, sizeof(gMouseButtonDown));
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    delete gFluid2D;
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void cursorPosCallback(GLFWwindow * window, double posX, double posY) {
    gMousePosition = glm::vec2(posX, posY);
}

void mouseButtonCallback(GLFWwindow * window, int button, int action, int mods) {
    switch (action)
    {
        case GLFW_PRESS:
            gMouseButtonDown[button] = true;
            gMouseButtonHeld[button] = true;
            break;
        case GLFW_RELEASE:
            gMouseButtonUp[button] = true;
            gMouseButtonHeld[button] = false;
            break;
        default:
            break;
    }
}
