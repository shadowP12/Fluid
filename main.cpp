#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include "Fluid2D.h"
#include "SPH.h"
#include "CommonUtil.h"

static Fluid2D* gFluid2D = nullptr;
static SPH* gSPH = nullptr;
static int gWidth = 512;
static int gHeight = 512;
static Input* gInput = nullptr;

void cursorPosCallback(GLFWwindow * window, double posX, double posY);
void mouseButtonCallback(GLFWwindow * window, int button, int action, int mods);
void mouseScrollCallback(GLFWwindow * window, double offsetX, double offsetY);

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    GLFWwindow* window = nullptr;
    window = glfwCreateWindow(gWidth, gHeight, "Fluid", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetScrollCallback(window, mouseScrollCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        LOGE("Failed to initialize GLAD");
        return -1;
    }
    gInput = new Input();
    memset(gInput->mouseButtonUp, 0, sizeof(gInput->mouseButtonUp));
    memset(gInput->mouseButtonHeld, 0, sizeof(gInput->mouseButtonHeld));
    memset(gInput->mouseButtonDown, 0, sizeof(gInput->mouseButtonDown));
    gInput->mouseScrollWheel = 0.0;

    gSPH = new SPH(gWidth, gHeight, gInput);
    while (!glfwWindowShouldClose(window)) {
        gSPH->tick(0.125);
        gInput->mouseLastPosition = gInput->mousePosition;
        memset(gInput->mouseButtonUp, 0, sizeof(gInput->mouseButtonUp));
        memset(gInput->mouseButtonDown, 0, sizeof(gInput->mouseButtonDown));
        gInput->mouseScrollWheel = 0.0;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    delete gFluid2D;
    delete gInput;
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void mouseScrollCallback(GLFWwindow * window, double offsetX, double offsetY)
{
    gInput->mouseScrollWheel = offsetY;
}

void cursorPosCallback(GLFWwindow * window, double posX, double posY) {
    gInput->mousePosition = glm::vec2(posX, posY);
}

void mouseButtonCallback(GLFWwindow * window, int button, int action, int mods) {
    switch (action)
    {
        case GLFW_PRESS:
            gInput->mouseButtonDown[button] = true;
            gInput->mouseButtonHeld[button] = true;
            break;
        case GLFW_RELEASE:
            gInput->mouseButtonUp[button] = true;
            gInput->mouseButtonHeld[button] = false;
            break;
        default:
            break;
    }
}
