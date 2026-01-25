#include <iostream>
#include "libs/SOIL2.h"
#include "GraphicalLibrary.hpp"

#define numVAOs 2
#define numVBOs 5

glm::mat4 mvMat;
glm::mat4 pMat;
glm::mat4 vMat;
glm::mat4 mMat;
GLuint projLoc;
GLuint mvLoc;
GLuint textureID;
GLuint cloudID;
GLuint starsID;
GLuint renderingProgram;
GLuint startsProgram;
GLuint vbo[numVBOs];
GLuint vao[numVAOs];
Sphere sphere(96);
fpsCounter* fps;
float aspect;
int width;
int height;
float rotationAngle;
float InclinaisonAngle;
bool IncInclinaisonAngle = true;
float numberOfFrames;
double timeData;
double* currentTime = &timeData;

void SetupVertices()
{
    vector<int> ind = sphere.getIndices();
    vector<glm::vec3> vert = sphere.getVertices();
    vector<glm::vec2> tex = sphere.getTexCoords();
    vector<glm::vec3> norm = sphere.getNormals();
    // vertex positions
    vector<float> pvalues;
    // texture coordinates
    vector<float> tvalues;
    // normal vectors
    vector<float> nvalues;
    int numIndices = sphere.getNumIndices();
    for (int i = 0; i < numIndices; i++)
    {
        pvalues.push_back((vert[ind[i]]).x);
        pvalues.push_back((vert[ind[i]]).y);
        pvalues.push_back((vert[ind[i]]).z);
        tvalues.push_back((tex[ind[i]]).s);
        tvalues.push_back((tex[ind[i]]).t);
        nvalues.push_back((norm[ind[i]]).x);
        nvalues.push_back((norm[ind[i]]).y);
        nvalues.push_back((norm[ind[i]]).z);
    }
    glGenVertexArrays(2, vao);
    glBindVertexArray(vao[0]);
    glGenBuffers(5, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);
    float rectanglesPosition[] = {
        -10.0f, -10.0f, -10.0f,
        10.0f, -10.0f, -10.0f,
        10.0f,  10.0f, -10.0f,
        -10.0f,  10.0f, -10.0f
    };
    float groundTexCoords[] = {
        0.0f,    0.0f,
        1.0f,    0.0f,    
        1.0f,    1.0f,
        0.0f,    1.0f
    };
    glBindVertexArray(vao[1]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectanglesPosition), rectanglesPosition, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundTexCoords), groundTexCoords, GL_STATIC_DRAW);
}

void init()
{
    renderingProgram = createShaderProgram("shaders/vertSphere.glsl", "shaders/fragSphere.glsl");
    startsProgram = createShaderProgram("shaders/vertStars.glsl", "shaders/fragStars.glsl");
    SetupVertices();
    textureID = SOIL_load_OGL_texture("assets/8k_earth_daymap.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    cloudID = SOIL_load_OGL_texture("assets/8k_earth_clouds.png", SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    starsID = SOIL_load_OGL_texture("assets/8k_stars_milky_way.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

}

void display(GLFWwindow* window)
{   
    fps->updateFPS();
    if (rotationAngle >= 360.0f) {
        rotationAngle = -1.0f;
    }
    rotationAngle += 1.0f;
    if (InclinaisonAngle >= 40.0f) {
        IncInclinaisonAngle = false;
    } else if (InclinaisonAngle <= -40.0f) {
        IncInclinaisonAngle = true;
    }
    if (IncInclinaisonAngle) {
        InclinaisonAngle += 0.1f;
    } else {
        InclinaisonAngle -= 0.1f;
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    aspect = (float)width / (float)height;
    // draw the background
    glUseProgram(startsProgram);
    mvLoc = glGetUniformLocation(startsProgram, "mv_matrix");
    projLoc = glGetUniformLocation(startsProgram, "proj_matrix");
    mvMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -0.5f));
    mvMat = glm::scale(mvMat, glm::vec3(1.0f, 1.0f, 0.0f));
    pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
    glBindVertexArray(vao[1]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glDisable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, starsID);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    // draw the earth
    glUseProgram(renderingProgram);
    projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
    mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
    auto pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
    vMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.5f));
    mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    mMat = glm::rotate(mMat, glm::radians(InclinaisonAngle), glm::vec3(1.0f, 0.0f, 0.0f));
    mMat = glm::rotate(mMat, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    mvMat = vMat * mMat;
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
    glBindVertexArray(vao[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDrawArrays(GL_TRIANGLES, 0, sphere.getNumIndices());
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, cloudID);
    glActiveTexture(GL_TEXTURE0);
    glDrawArrays(GL_TRIANGLES, 0, sphere.getNumIndices());
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow *window = glfwCreateWindow(1000, 1000, "earth", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glViewport(0, 0, 1000, 1000);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glewInit();
    init();
    rotationAngle = 0;
    InclinaisonAngle = 0;
    fps = new fpsCounter(window, currentTime);
    while (!glfwWindowShouldClose(window))
    {
        timeData = glfwGetTime();
        display(window);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}