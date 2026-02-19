#include <cmath>
#include <vector>
#include <iostream>
#include "libs/SOIL2.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <string>
#include <sstream>
#include <charconv>

using namespace std;

class Sphere
{
private:
    int numVertices;
    int numIndices;
    vector<int> indices;
    vector<glm::vec3> vertices;
    vector<glm::vec2> texCoords;
    vector<glm::vec3> normals;

public:
    Sphere(int precision)
    {
        numVertices = (precision + 1) * (precision + 1);
        numIndices = precision * precision * 6;
        for (int i = 0; i < numVertices; i++)
        {
            vertices.push_back(glm::vec3());
            texCoords.push_back(glm::vec2());
            normals.push_back(glm::vec3());
        }
        for (int i = 0; i < numIndices; i++)
        {
            indices.push_back(0);
        }
        // calculate triangle vertices
        for (int i = 0; i <= precision; i++)
        {
            for (int j = 0; j <= precision; j++)
            {
                float y = (float)cos(glm::radians(180.0f - i * 180.0f / precision));
                float x = -(float)cos(glm::radians(j * 360.0f / precision)) * (float)abs(cos(asin(y)));
                float z = (float)sin(glm::radians(j * 360.0f / precision)) * (float)abs(cos(asin(y)));
                vertices[i * (precision + 1) + j] = glm::vec3(x, y, z);
                texCoords[i * (precision + 1) + j] = glm::vec2(((float)j / precision), ((float)i / precision));
                normals[i * (precision + 1) + j] = glm::vec3(x, y, z);
            }
        }
        // calculate triangle indices
        for (int i = 0; i < precision; i++)
        {
            for (int j = 0; j < precision; j++)
            {
                indices[6 * (i * precision + j) + 0] = i * (precision + 1) + j;
                indices[6 * (i * precision + j) + 1] = i * (precision + 1) + j + 1;
                indices[6 * (i * precision + j) + 2] = (i + 1) * (precision + 1) + j;
                indices[6 * (i * precision + j) + 3] = i * (precision + 1) + j + 1;
                indices[6 * (i * precision + j) + 4] = (i + 1) * (precision + 1) + j + 1;
                indices[6 * (i * precision + j) + 5] = (i + 1) * (precision + 1) + j;
            }
        }
    }
    int getNumVertices()
    {
        return numVertices;
    }
    int getNumIndices()
    {
        return numIndices;
    }
    vector<int> getIndices()
    {
        return indices;
    }
    vector<glm::vec3> getVertices()
    {
        return vertices;
    }
    vector<glm::vec2> getTexCoords()
    {
        return texCoords;
    }
    vector<glm::vec3> getNormals()
    {
        return normals;
    }
};

GLuint createShaderProgram(string PathToVert, string PathToFrag)
{
    // read the shader files
    string vShaderSource = "";
    string fShaderSource = "";
    ifstream vertFile(PathToVert, ios::in);
    string line = "";
    while (!vertFile.eof())
    {
        getline(vertFile, line);
        vShaderSource.append(line + "\n");
    }
    vertFile.close();
    ifstream fragFile(PathToFrag, ios::in);
    while (!fragFile.eof())
    {
        getline(fragFile, line);
        fShaderSource.append(line + "\n");
    }
    fragFile.close();
    // compile it, link it, and return it
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char *vSourcePtr = vShaderSource.c_str();
    const char *fSourcePtr = fShaderSource.c_str();
    glShaderSource(vShader, 1, &vSourcePtr, nullptr);
    glShaderSource(fShader, 1, &fSourcePtr, nullptr);
    glCompileShader(vShader);
    glCompileShader(fShader);
    GLuint vfProgram = glCreateProgram();
    glAttachShader(vfProgram, vShader);
    glAttachShader(vfProgram, fShader);
    glLinkProgram(vfProgram);
    if (vfProgram < 0)
    {
        cout << "critical error : compilation failed with exit code : " << vfProgram << endl;
    }
    return vfProgram;
}

class fpsCounter
{
private:
    GLFWwindow *window;
    double lastTime;
    double framesPassed;
    double *currentTime;
    string DefaultTitle;
public:
    fpsCounter(GLFWwindow *win, double *time, string title)
    {
        window = win;
        lastTime = glfwGetTime();
        currentTime = time;
        DefaultTitle = title;
    }
    void updateFPS()
    {
        framesPassed++;
        if ((*currentTime - lastTime) > 1.0f)
        {
            double fps = framesPassed / (*currentTime - lastTime);
            string title = DefaultTitle + " (FPS : " + to_string(fps) + ")";
            glfwSetWindowTitle(window, title.c_str());
            lastTime = *currentTime;
            framesPassed = 0;
        }
    }
};

class SingleObject {
    private:
        GLuint objectProgram;
        GLuint* textureID;
        int texNumber = 0;
        GLuint mvLoc;
        GLuint projLoc;
    public:
        int numVertices = 0;
        vector<int> indices;
        vector<glm::vec3> vertices;
        vector<glm::vec2> texCoords;
        vector<glm::vec3> normals;
        glm::mat4 mvMat, vMat, mMat, pMat;
        GLuint vao[1];
        GLuint vbo[3];
        SingleObject(string vertPath, string fragPath, string ObjectPath, float aspect)
        {
            // initialize the gpu program
            objectProgram = createShaderProgram(vertPath, fragPath);
            // initialize the camera position
            glUseProgram(objectProgram);
            projLoc = glGetUniformLocation(objectProgram, "proj_matrix");
            mvLoc = glGetUniformLocation(objectProgram, "mv_matrix");
            pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
            vMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.5f));
            mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
            mvMat = vMat * mMat;
            // now the essential step : parse the blender (or other) obj file
            fstream ObjectFile(ObjectPath);
            string line;
            while (getline(ObjectFile, line))
            {
                stringstream ss(line);
                vector<string> tokens;
                string token;
                while (getline(ss, token, ' '))
                {
                    tokens.push_back(token);
                }
                if (tokens.size() > 0)
                {
                    if (tokens[0] == "#" || tokens[0] == "mtllib" || tokens[0] == "s") {
                    continue;
                    }
                    else if (tokens[0] == "v") {
                        if (tokens.size() < 4) {
                            cout << "critical error : invalid entry in an object file." << endl;
                            continue;
                        }
                        glm::vec3 tmp(stof(tokens[1]), stof(tokens[2]), stof(tokens[3]));
                        vertices.push_back(tmp);
                        numVertices++;
                    } else if (tokens[0] == "vn") {
                        if (tokens.size() < 4) {
                            cout << "critical error : invalid entry in an object file." << endl;
                            continue;
                        }
                        glm::vec3 tmp(stof(tokens[1]), stof(tokens[2]), stof(tokens[3]));
                        normals.push_back(tmp);
                    } else if (tokens[0] == "vt") {
                        if (tokens.size() < 3) {
                            cout << "critical error : invalid entry in an object file." << endl;
                            continue;
                        }
                        glm::vec2 tmp(stof(tokens[1]), stof(tokens[2]));
                        texCoords.push_back(tmp);
                    } else if (tokens[0] == "f") {
                        // check that the number of tokens above two
                        if (tokens.size() < 4) {
                            cout << "critical error : invalid entry in an object file." << endl;
                            continue;
                        }
                        // split the tokens in numbers, and do the operations
                        if (tokens.size() - 1 == 3) {
                            for (int i = 1; i < tokens.size(); i++) {
                                string number;
                                stringstream sn(tokens[i]);
                                getline(sn, number, '/');
                                int x;
                                from_chars(number.data(), number.data() + number.size(), x);
                                x--;
                                indices.push_back(x);
                            }
                        } else if (tokens.size() - 1 == 4) {
                            for (int i = 1; i <= 3; i++) {
                                string number;
                                stringstream sn(tokens[i]);
                                getline(sn, number, '/');
                                int x;
                                from_chars(number.data(), number.data() + number.size(), x);
                                x--;
                                indices.push_back(x);
                            }
                            for (int i = 1; i <= 4; i++) {
                                if (i == 2) {
                                    i++;
                                }
                                string number;
                                stringstream sn(tokens[i]);
                                getline(sn, number, '/');
                                int x;
                                from_chars(number.data(), number.data() + number.size(), x);
                                x--;
                                indices.push_back(x);
                            }
                        }
                    }
                }
            }
        }
        void UpdateAspect(float aspect) {
            pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
        }
        void initTexture(string pathOfTheTexture, int flag1, unsigned int flag2, unsigned int flag3) {
            textureID = new GLuint[1];
            textureID[0] = SOIL_load_OGL_texture(pathOfTheTexture.c_str(), flag1, flag2, flag3);
            texNumber++;
        }
        void initTextures(int numberOfTextures, vector<string> pathOfTextures, vector<int> flag1, vector<unsigned int> flag2, vector<unsigned int> flag3) {
            if (pathOfTextures.size() < numberOfTextures || flag1.size() < numberOfTextures || flag2.size() < numberOfTextures || flag3.size() < numberOfTextures) {
                cout << "critical error : no enough texture files provided." << endl;
                return;
            }
            textureID = new GLuint[numberOfTextures];
            for (int i = 0; i < numberOfTextures; i++) {
                textureID[i] = SOIL_load_OGL_texture(pathOfTextures[i].c_str(), flag1[i], flag2[i], flag3[i]);
                texNumber++;
            }
        }
        void initVertexGeneric() {
            glGenVertexArrays(1, vao);
            glBindVertexArray(vao[0]);
            glGenBuffers(3, vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
            glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), &texCoords[0], GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
            glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
        }
        void PrepDisplayObjectGeneric() {
            glUseProgram(objectProgram);
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
        }
        void displayTextures() {
            for (int i = 0; i < texNumber; i++) {
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, textureID[i]);
            }
            glEnable(GL_DEPTH_TEST);
        } 
        void GenericDraw() {
            for (int i = 0; i < texCoords.size(); i++) {
                cout << i << " : " << normals[i].x << " " << normals[i].y << " " << normals[i].z << endl;
            }
            glDrawArrays(GL_TRIANGLES, 0, vertices.size());
        }
};