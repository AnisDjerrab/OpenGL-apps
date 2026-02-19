#include <iostream>
#include "GraphicalLibrary.hpp"

float aspect;
int width;
int height;
SingleObject* object;
fpsCounter* fps;
double timeData;
double* currentTime = &timeData;

void init() {
    object = new SingleObject("shaders/vertBlender.glsl", "shaders/fragBlender.glsl", "blenderObjects/basic.obj", 1.0f);
    object->initTexture("assets/sand.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    object->initVertexGeneric();
}

void display(GLFWwindow* window) {
    fps->updateFPS();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwGetFramebufferSize(window, &width, &height);
    aspect = (float)width / (float)height;
    glViewport(0, 0, width, height);
    object->UpdateAspect(aspect);
    object->PrepDisplayObjectGeneric();
    object->displayTextures();
    object->GenericDraw();

}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(800, 800, "BlenderTexture", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glViewport(0, 0, 800, 800);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glewInit();
    init();
    fps = new fpsCounter(window, currentTime, "BlenderTexture");
    while (!glfwWindowShouldClose(window)) {
        timeData = glfwGetTime();
        display(window);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}