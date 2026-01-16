#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>

#define numVAOs 1

using namespace std;

GLuint renderingProgram;
GLuint vao[numVAOs];
float offset_x = 0;
float offset_y = 0;
bool add_x = true;
bool add_y = false;

GLuint createShaderProgram() {
    // read the shader files
    string vShaderSource = "";
    string fShaderSource = "";
    ifstream vertFile("shaders/vertTriangle.glsl", ios::in);
    string line = "";
    while (!vertFile.eof()) {
        getline(vertFile, line);
        vShaderSource.append(line + "\n");
    }
    vertFile.close();
    ifstream fragFile("shaders/fragTriangle.glsl", ios::in);
    while (!fragFile.eof()) {
        getline(fragFile, line);
        fShaderSource.append(line + "\n");
    }
    fragFile.close();
    // compile it, link it, and return it
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* vSourcePtr = vShaderSource.c_str();
    const char* fSourcePtr = fShaderSource.c_str();
    glShaderSource(vShader, 1, &vSourcePtr, nullptr);
    glShaderSource(fShader, 1, &fSourcePtr, nullptr);
    glCompileShader(vShader);
    glCompileShader(fShader);
    GLuint vfProgram = glCreateProgram();
    glAttachShader(vfProgram, vShader);
    glAttachShader(vfProgram, fShader);
    glLinkProgram(vfProgram);
    return vfProgram;
}

void init() {
    renderingProgram = createShaderProgram();
    glGenVertexArrays(numVAOs, vao);
    glPointSize(10.0f);
    glBindVertexArray(vao[0]);
}

void display(GLFWwindow* window, double currentTime) {
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(renderingProgram);
    GLint offset_x_gpu = glGetUniformLocation(renderingProgram, "offset_x");
    GLint offset_y_gpu = glGetUniformLocation(renderingProgram, "offset_y");
    glUniform1f(offset_x_gpu, offset_x / 200.0f);
    glUniform1f(offset_y_gpu, offset_y / 200.0f);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(800, 800, "triangles", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glViewport(0, 0, 800, 800);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glewInit();
    init();
    while (!glfwWindowShouldClose(window)) {
        if (offset_x > 199.0f) {
            add_x = false;
            offset_x -= 1.14f;
        } else if (offset_x < -199.0f) {
            add_x = true;
            offset_x += 1.17f;
        } else {
            if (add_x) {
                offset_x += 1.07f;
            } else {
                offset_x -= 1.21f;
            }
        }
        if (offset_y > 198.9f) {
            add_y = false;
            offset_y -= 1.44f;
        } else if (offset_y < -198.9f) {
            add_y = true;
            offset_y += 0.89f;
        } else {
            if (add_y) {
                offset_y += 1.23f;
            } else {
                offset_y -= 1.75f;
            }
        }
        display(window, glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}