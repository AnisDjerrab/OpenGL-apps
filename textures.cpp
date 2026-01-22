#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "libs/SOIL2.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <fstream>

#define numVAOs 2
#define numVBOs 4

using namespace std;

GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];
GLuint textureID;
GLuint sandID;
int width, height;
float aspect;

void setupVertices(void) {
    float vertexPositions[54] = { -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 
        1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f
    };
    float TexCoords[36] = {
    0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
    0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
    0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f };
    float rectanglesPosition[12] {
        -1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f
    };
    float groundTexCoords[] = {
        0.0f,    0.0f,
        8.0f,    0.0f,    
        8.0f,    8.0f,
        0.0f,    8.0f
    };
    glGenVertexArrays(2, vao);
    glBindVertexArray(vao[0]);
    glGenBuffers(numVBOs, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords), TexCoords, GL_STATIC_DRAW);
    glBindVertexArray(vao[1]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectanglesPosition), rectanglesPosition, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundTexCoords), groundTexCoords, GL_STATIC_DRAW);
}

GLuint createShaderProgram() {
    // read the shader files
    string vShaderSource = "";
    string fShaderSource = "";
    ifstream vertFile("shaders/vertTexture.glsl", ios::in);
    string line = "";
    while (!vertFile.eof()) {
        getline(vertFile, line);
        vShaderSource.append(line + "\n");
    }
    vertFile.close();
    ifstream fragFile("shaders/fragTexture.glsl", ios::in);
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
    // load the brick wall texture
    textureID = SOIL_load_OGL_texture("assets/brick_wall.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    sandID = SOIL_load_OGL_texture("assets/sand.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);  
    glBindTexture(GL_TEXTURE_2D, sandID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void display(GLFWwindow* window, double currentTime) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(renderingProgram);
    glfwGetFramebufferSize(window, &width, &height);
    aspect = (float)width / (float)height;
    auto mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
    auto projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
    auto mvMat = glm::mat4(1.0f);
    mvMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f, -5.0f));
    auto pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDrawArrays(GL_TRIANGLES, 0, 18);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
    glActiveTexture(GL_TEXTURE1);
    mvMat = glm::translate(glm::mat4(1.0f), glm::vec3(-500.0f, 500.0f, -1500.0f));
    mvMat = glm::rotate(mvMat, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    mvMat = glm::scale(mvMat, glm::vec3(1000.0f, 1.0f, 1000.0f));
    glBindTexture(GL_TEXTURE_2D, sandID);
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
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
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}