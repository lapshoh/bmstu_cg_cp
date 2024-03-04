#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "model.h"

#define GLT_IMPLEMENTATION
#include "gltext.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1800;
const unsigned int SCR_HEIGHT = 1000;

// camera
Camera camera(glm::vec3(10.0f, 2.0f, 0.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 lightPos(10000.0f, 10000.0f, 10000.0f);

vector < glm::mat4 > mtr;
vector < Model > objects;
bool arr_flag = false;
float speed = 45.0f;
float time_fl = 0.0;
float angle = glm::radians(0.0f);

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Archery", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);


    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);


    Shader ourShader("C:/Users/lapsh/source/repos/course_pr/shader.vert", "C:/Users/lapsh/source/repos/course_pr/shader.frag");
    Shader lightingShader("C:/Users/lapsh/source/repos/course_pr/basic_lighting.vert", "C:/Users/lapsh/source/repos/course_pr/basic_lighting.frag");

    Model ourModel_1("C:/Users/lapsh/source/repos/course_pr/Arrow.obj");
    Model ourModel_2("C:/Users/lapsh/source/repos/course_pr/Bow.obj");
    Model ourModel_3("C:/Users/lapsh/source/repos/course_pr/Target.obj");
    Model ourModel_4("C:/Users/lapsh/source/repos/course_pr/Grass.obj");

    objects.push_back(ourModel_2);
    objects.push_back(ourModel_3);
    objects.push_back(ourModel_4);
    objects.push_back(ourModel_1);

    glm::mat4 model_st;
    model_st = glm::translate(glm::mat4(1.0f), glm::vec3(0.12f, 2.0f, 0.0f));
    model_st = glm::scale(model_st, glm::vec3(0.5f, 1.0f, 1.0f));
    model_st = glm::rotate(model_st, glm::radians(5.0f), glm::vec3(-1.0, 0.0, 0.0));
    mtr.push_back(model_st);
    model_st = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 50.0f));
    mtr.push_back(model_st);
    model_st = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)); 
    model_st = glm::scale(model_st, glm::vec3(10.0f, 1.0f, 10.0f));	
    mtr.push_back(model_st);
    model_st = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f, -0.2f));
    mtr.push_back(model_st);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);


    if (!gltInit())
    {
        fprintf(stderr, "Failed to initialize glText\n");
        glfwTerminate();
        return EXIT_FAILURE;
    }

    GLTtext* text1 = gltCreateText();
    GLTtext* text2 = gltCreateText();
    GLTtext* text3 = gltCreateText();

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.6f, 0.7f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightingShader.use();
        lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("lightPos", lightPos);
        lightingShader.setVec3("viewPos", camera.Position);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        lightingShader.setMat4("model", model);

        ourShader.use();

        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        if (arr_flag == true)
        {
            if ((!(mtr[mtr.size() - 1][3][2] > mtr[1][3][2] - 1.0 && mtr[mtr.size() - 1][3][2] < mtr[1][3][2] + 0.2
                && mtr[mtr.size() - 1][3][1] < 3.95f && mtr[mtr.size() - 1][3][1] > 1.55f))
                && mtr[mtr.size() - 1][3][1] > 0.001f)
            {
                glm::mat4 model = glm::mat4(1.0f);
                float ux = speed * cos(angle);
                float uy = speed * sin(angle) - 9.81 * time_fl;
                float tgx = float(abs(uy) / ux); 
                float tgy = float(abs(uy + 0.00981) / ux);
                float delta = atan(float((tgx - tgy) / (1 + tgx * tgy)));
                model *= glm::rotate(glm::mat4(1.0f), delta, glm::vec3(1.0, 0.0, 0.0));
                model *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0, uy * 0.001, ux * 0.001));

                mtr[mtr.size() - 1] *= model;
            }
            else
            {
                arr_flag = false;
                objects.push_back(ourModel_1);
                model_st = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f, -0.2f));
                model_st = glm::rotate(model_st, float(angle), glm::vec3(-1.0, 0.0, 0.0));
                mtr.push_back(model_st);
            }
        }
        else
            time_fl = 0.0f;

        for (int i = 0; i < mtr.size(); ++i)
        {
            ourShader.setMat4("model", mtr[i]);
            objects[i].Draw(ourShader);
        }

        time_fl += 0.001;

        gltSetText(text1, ("Dist to target = " + std::to_string(int(mtr[1][3][2]))).c_str());
        gltSetText(text2, ("Arrow speed = " + std::to_string(int(speed))).c_str());
        gltSetText(text3, ("Bow angle = " + std::to_string(int(glm::degrees(angle)))).c_str());

        gltBeginDraw();

        gltColor(1.0f, 1.0f, 1.0f, 1.0f);
        gltDrawText2D(text1, 0.0f, 0.0f, 1.0f);
        gltDrawText2D(text2, 0.0f, 20.0f, 1.0f);
        gltDrawText2D(text3, 0.0f, 40.0f, 1.0f);

        gltEndDraw();


        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    gltDeleteText(text1);
    gltDeleteText(text2);
    gltDeleteText(text3);

    gltTerminate();
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        arr_flag = true;
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
    {
        if (mtr[1][3][2] < 70.0)
        {
            mtr[1][3][2] += 0.01f;
            for (int i = 3; i < mtr.size() - 1; ++i)
                if (mtr[i][3][1] > 0.001f)
                    mtr[i][3][2] += 0.01f;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        if (mtr[1][3][2] > 30.3)
        {
            mtr[1][3][2] -= 0.01f;
            for (int i = 3; i < mtr.size() - 1; ++i)
                if (mtr[i][3][1] > 0.001f)
                    mtr[i][3][2] -= 0.01f;

        }
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
    {
        if (speed < 65.0)
        {
            speed += 0.01;
            glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f - abs(45.0 - speed) * 0.00001, 
                1.0f + abs(45.0 - speed) * 0.00001f));
            mtr[0] *= model;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
    {
        if (speed > 25.3)
        {
            speed -= 0.01;
            glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f + abs(45.0 - speed) * 0.00001, 
                1.0f - abs(45.0 - speed) * 0.00001f));
            mtr[0] *= model;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(0.01f), glm::vec3(-1.0, 0.0, 0.0));
        if (int(glm::degrees(angle)) < 30 && arr_flag == false)
        {
            mtr[0] *= model;
            mtr[mtr.size() - 1] *= model;
            angle += glm::radians(0.01f);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(0.01f), glm::vec3(1.0, 0.0, 0.0));
        if (int(glm::degrees(angle)) > -30 && arr_flag == false)
        {
            mtr[0] *= model;
            mtr[mtr.size() - 1] *= model;
            angle -= glm::radians(0.01f);
        }
    }
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}