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
#define numVBOs 5

using namespace std;

GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];
GLuint textureID;
GLuint sandID;
int width, height;
float aspect;

void setupVertices(void) {
    float vertexPositions[54] = { -75.0f, -75.0f, 75.0f, 75.0f, -75.0f, 75.0f, 0.0f, 75.0f, 0.0f,
        75.0f, -75.0f, 75.0f, 75.0f, -75.0f, -75.0f, 0.0f, 75.0f, 0.0f,
        75.0f, -75.0f, -75.0f, -75.0f, -75.0f, -75.0f, 0.0f, 75.0f, 0.0f,
        -75.0f, -75.0f, -75.0f, -75.0f, -75.0f, 75.0f, 0.0f, 75.0f, 0.0f,
        -75.0f, -75.0f, -75.0f, 75.0f, -75.0f, 75.0f, -75.0f, -75.0f, 75.0f, 
        75.0f, -75.0f, 75.0f, -75.0f, -75.0f, -75.0f, 75.0f, -75.0f, -75.0f
    };
    float vertexPositionsSmall[54] = { -45.0f, -45.0f, 45.0f, 45.0f, -45.0f, 45.0f, 0.0f, 45.0f, 0.0f,
        45.0f, -45.0f, 45.0f, 45.0f, -45.0f, -45.0f, 0.0f, 45.0f, 0.0f,
        45.0f, -45.0f, -45.0f, -45.0f, -45.0f, -45.0f, 0.0f, 45.0f, 0.0f,
        -45.0f, -45.0f, -45.0f, -45.0f, -45.0f, 45.0f, 0.0f, 45.0f, 0.0f,
        -45.0f, -45.0f, -45.0f, 45.0f, -45.0f, 45.0f, -45.0f, -45.0f, 45.0f, 
        45.0f, -45.0f, 45.0f, -45.0f, -45.0f, -45.0f, 45.0f, -45.0f, -45.0f
    };
    float TexCoords[36] = {
    0.0f, 0.0f, 9.0f, 0.0f, 0.5f, 9.0f,
    0.0f, 0.0f, 9.0f, 0.0f, 0.5f, 9.0f,
    0.0f, 0.0f, 9.0f, 9.0f, 0.0f, 9.0f,
    0.0f, 0.0f, 9.0f, 0.0f, 0.5f, 9.0f,
    0.0f, 0.0f, 9.0f, 0.0f, 0.5f, 9.0f,
    0.0f, 0.0f, 9.0f, 9.0f, 0.0f, 9.0f };
    float rectanglesPosition[12] {
        -1000.0f, 0.0f, -1000.0f,
        1000.0f, 0.0f, -1000.0f,
        1000.0f, 0.0f,  1000.0f,
        -1000.0f, 0.0f,  1000.0f
    };
    float groundTexCoords[] = {
        0.0f,    0.0f,
        100.0f,    0.0f,    
        100.0f,    100.0f,
        0.0f,    100.0f
    };
    glGenVertexArrays(2, vao);
    glBindVertexArray(vao[0]);
    glGenBuffers(numVBOs, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords), TexCoords, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositionsSmall), vertexPositionsSmall, GL_STATIC_DRAW);
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
}

void display(GLFWwindow* window, double currentTime) {
    glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(renderingProgram);
    glfwGetFramebufferSize(window, &width, &height);
    aspect = (float)width / (float)height;
    auto mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
    auto projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
    auto mvMat = glm::mat4(1.0f);
    mvMat = glm::translate(mvMat, glm::vec3(75.0f, 100.0f, -345.0f));
    mvMat = glm::rotate(mvMat, glm::radians(10.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    mvMat = glm::rotate(mvMat, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
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
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    float anisoSetting = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisoSetting);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDrawArrays(GL_TRIANGLES, 0, 18);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]); 
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
    mvMat = glm::mat4(1.0f);
    mvMat = glm::translate(mvMat, glm::vec3(0.0f, 60.0f, -245.0f));
    mvMat = glm::rotate(mvMat, glm::radians(10.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    mvMat = glm::rotate(mvMat, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
    glDrawArrays(GL_TRIANGLES, 0, 18);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]); 
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
    mvMat = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, -5.5f, -5.0f));
    mvMat = glm::rotate(mvMat, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    mvMat = glm::rotate(mvMat, glm::radians(10.0f), glm::vec3(1.0f, 0.0f, 1.0f));
    glBindTexture(GL_TEXTURE_2D, sandID);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    anisoSetting = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisoSetting);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisoSetting);
    glActiveTexture(GL_TEXTURE0);
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