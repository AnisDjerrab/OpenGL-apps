#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <random>
#include <vector>
#include <thread>

#define numVAOs 1
#define numVBOs 2

using namespace std;

GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];
GLuint InstanceVBO;
GLuint mvLoc, projLoc;
int width, height;
float aspect;
glm::mat4 pmat, vMat, mMat, mvMat;
float degree = 0;
float cubes[50000];
float cubes_x[50000];
float cubes_y[50000];
float cubes_z[50000];
float velocityDirection[50000][3];
float rotationChange[50000][3];
float rotation_x[50000];
float rotation_y[50000];
float rotation_z[50000];

void setupVertices(void) {
    float vertexPositions[108] = {
        -0.5f, 0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f,
        0.5f, -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, -0.5f,
        0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, -0.5f,
        0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.5f,
        -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f,
    };
    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);
    glGenBuffers(numVBOs, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
}

GLuint createShaderProgram() {
    // read the shader files
    string vShaderSource = "";
    string fShaderSource = "";
    ifstream vertFile("shaders/vertCube.glsl", ios::in);
    string line = "";
    while (!vertFile.eof()) {
        getline(vertFile, line);
        vShaderSource.append(line + "\n");
    }
    vertFile.close();
    ifstream fragFile("shaders/fragCube.glsl", ios::in);
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
    setupVertices();
    for (int i = 0; i < 50000; i++) {
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<float> dis(0.0f, 1.0f);
        cubes[i] = dis(gen);
        uniform_real_distribution<float> negative(-25.0f, 0.0);
        uniform_real_distribution<float> positive(0.0, 25);
        if (uniform_int_distribution<int>(0, 1)(gen) == 0) {
            cubes_x[i] = negative(gen);
        } else {
            cubes_x[i] = positive(gen);
        }
        if (uniform_int_distribution<int>(0, 1)(gen) == 0) {
            cubes_y[i] = negative(gen);
        } else {
            cubes_y[i] = positive(gen);
        }
        uniform_real_distribution<float> real(-200, -25);
        cubes_z[i] = real(gen);
        uniform_real_distribution<float> velocity(-0.05, 0.05);
        velocityDirection[i][0] = velocity(gen);
        velocityDirection[i][1] = velocity(gen);
        velocityDirection[i][2] = velocity(gen);
        uniform_real_distribution<float> angle(0, 5);
        rotation_x[i] = angle(gen);
        rotation_y[i] = angle(gen);
        rotation_z[i] = angle(gen);
        uniform_real_distribution<float> DeltaRotation(-1, 1);
        rotationChange[i][0] = DeltaRotation(gen);
        rotationChange[i][1] = DeltaRotation(gen);
        rotationChange[i][2] = DeltaRotation(gen);
    }  
    glGenBuffers(1, &InstanceVBO);
    glBindVertexArray(vao[0]);
    glBindBuffer(GL_ARRAY_BUFFER, InstanceVBO);
    for(int i = 0; i < 4; i++)
    {
        GLuint loc = 3 + i;
        glEnableVertexAttribArray(loc);
        glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * sizeof(glm::vec4)));
        glVertexAttribDivisor(loc, 1); 
    }

    glBindVertexArray(0);
}

void display(GLFWwindow* window, double currentTime) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(renderingProgram);
    projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
    glfwGetFramebufferSize(window, &width, &height);
    aspect = (float)width / (float)height;
    int i = 0;
    int o = 0;
    for (i; i < 50000; i++) {
        auto pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
        vector<glm::mat4> instantMatrices(5000);
        for (i; i < (5000 + o*5000); i++) {
            random_device rd;
            mt19937 gen(rd());
            uniform_real_distribution<float> dis(-0.001, 0.001);
            velocityDirection[i][0] += dis(gen);
            velocityDirection[i][1] += dis(gen);
            velocityDirection[i][2] += dis(gen);
            cubes_x[i] += velocityDirection[i][0];
            cubes_y[i] += velocityDirection[i][1];
            cubes_z[i] += velocityDirection[i][2];
            vMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
            mMat = glm::translate(glm::mat4(1.0f), glm::vec3(cubes_x[i], cubes_y[i], cubes_z[i]));
            mMat = glm::scale(mMat, glm::vec3(cubes[i]));
            uniform_real_distribution<float> DeltaRotation(-0.2f, 0.2f);
            rotationChange[i][0] += DeltaRotation(gen);
            rotationChange[i][1] += DeltaRotation(gen);
            rotationChange[i][2] += DeltaRotation(gen);
            rotation_x[i] += rotationChange[i][0];
            rotation_y[i] += rotationChange[i][1];
            rotation_z[i] += rotationChange[i][2];
            mMat = glm::rotate(mMat, glm::radians(rotation_x[i]), glm::vec3(1.0f, 0.0f, 0.0f));
            mMat = glm::rotate(mMat, glm::radians(rotation_y[i]), glm::vec3(0.0f, 1.0f, 0.0f));
            mvMat = vMat * mMat;
            instantMatrices[i - o*5000] = mvMat;
        }
        glBindBuffer(GL_ARRAY_BUFFER, InstanceVBO);
        glBufferData(GL_ARRAY_BUFFER, 5000 * sizeof(glm::mat4), instantMatrices.data(), GL_DYNAMIC_DRAW);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glBindVertexArray(vao[0]);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, 5000);
        o++;
    }
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(800, 800, "cube", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glViewport(0, 0, 800, 800);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glewInit();
    init();
    while (!glfwWindowShouldClose(window)) {
        display(window, glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();
        degree++;
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}