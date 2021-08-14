// To build app run 'cmake .. ; cmake --build . --config Release; ./Release/LearnOpenGL.exe' from build folder for Windows
// To build app run 'cmake .. ; cmake --build . --config Release; ./LearnOpenGL' from build folder for MacOS
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "./Shader/Shader.h"
#include "./Camera/Camera.h"
#include "./stb_image/stb_image.h"
#include "./models/Cube/Cube.h"
#include <glm/gtx/string_cast.hpp>

bool filled = true;
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow *window, Camera &camera, float deltaTime);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);

float textureLevel = 0.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

unsigned int SCR_WIDTH = 1920;
unsigned int SCR_HEIGHT = 1080;

Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

bool firstMouse = true;

glm::vec3 *mainCubePosition = new glm::vec3(0);
glm::vec3 lightingPos(10.2f, 6.0f, 6.0f);
glm::vec3 lightPos(5.2f, 6.0f, 6.0f);

int main()
{
    // glfw: initialization and configuration
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw: window creation
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    // glad: loading opengl pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Shaders creation
    Shader shader("shaders/vertex/vertexShaderSource.vs", "shaders/fragment/fragmentShaderSource.fs");
    Shader lightShader("shaders/vertex/lightVertexShaderSource.vs", "shaders/fragment/lightFragmentShaderSource.fs");
    Shader lightingShader("shaders/vertex/lightingVertexShaderSource.vs", "shaders/fragment/lightingFragmentShaderSource.fs");

    glm::vec3 cubePositions[] = {
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)};

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO); // мы также можем генерировать несколько VAO или буферов одновременно
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Configuring first triangle
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(CubeModel::indices), CubeModel::indices, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CubeModel::vertices), CubeModel::vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

    // Загрузка и создание текстуры
    stbi_set_flip_vertically_on_load(true);
    unsigned int textures[2];
    glGenTextures(2, textures);
    glBindTexture(GL_TEXTURE_2D, textures[0]); // все последующие GL_TEXTURE_2D-операции теперь будут влиять на данный текстурный объект

    // Установка параметров наложения текстуры
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // установка метода наложения текстуры GL_REPEAT (стандартный метод наложения)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Установка параметров фильтрации текстуры
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Загрузка изображения, создание текстуры и генерирование мипмап-уровней
    int width, height, nrChannels;
    unsigned char *data = stbi_load("textures/marina.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // Second texture
    glBindTexture(GL_TEXTURE_2D, textures[1]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    data = stbi_load("textures/awesomeface.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    shader.use();

    shader.setInt("box_texture", 0);
    shader.setInt("smile_texture", 1);

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        // Delta time calculation
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textures[1]);

        processInput(window, camera, deltaTime);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        glm::mat3 normalMatrix = glm::mat3(1.0f);

        model = glm::translate(model, *mainCubePosition);
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));

        normalMatrix = glm::mat3(glm::transpose(glm::inverse(camera.GetViewMatrix() * model)));

        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);

        unsigned int modelLoc = glGetUniformLocation(shader.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        unsigned int viewLoc = glGetUniformLocation(shader.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));
        shader.setMat4("projection", projection);
        shader.setFloat("textureLevel", textureLevel);
        shader.setVec3("lightPos", lightPos);
        shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        shader.setMat3("normalMatrix", normalMatrix);
        shader.setVec3("viewPos", camera.position);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        lightShader.use();
        model = glm::translate(glm::mat4(1.0f), lightPos);
        unsigned int lightModelLoc = glGetUniformLocation(lightShader.ID, "model");
        glUniformMatrix4fv(lightModelLoc, 1, GL_FALSE, glm::value_ptr(model));
        unsigned int lightViewLoc = glGetUniformLocation(lightShader.ID, "view");
        glUniformMatrix4fv(lightViewLoc, 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));
        lightShader.setMat4("projection", projection);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        lightingShader.use();
        lightingShader.setVec3("lightPos", lightPos);
        model = glm::translate(glm::mat4(1.0f), lightingPos);
        normalMatrix = glm::mat3(glm::transpose(glm::inverse(camera.GetViewMatrix() * model)));
        unsigned int lightingModelLoc = glGetUniformLocation(lightingShader.ID, "model");
        glUniformMatrix4fv(lightingModelLoc, 1, GL_FALSE, glm::value_ptr(model));
        unsigned int lightingViewLoc = glGetUniformLocation(lightingShader.ID, "view");
        glUniformMatrix4fv(lightingViewLoc, 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));
        lightingShader.setMat4("projection", projection);
        lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        lightingShader.setMat3("normalMatrix", normalMatrix);
        lightingShader.setVec3("viewPos", camera.position);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        for (int i = 0; i < *(&cubePositions + 1) - cubePositions; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            if ((i + 1) % 3 == 0)
            {
                model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 0.0f, 0.0f));
            }
            else
            {
                model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
            }
            normalMatrix = glm::mat3(glm::transpose(glm::inverse(camera.GetViewMatrix() * model)));
            shader.use();
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));
            shader.setVec3("lightPos", lightPos);
            shader.setMat4("projection", projection);
            shader.setFloat("textureLevel", textureLevel);
            shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
            shader.setMat3("normalMatrix", normalMatrix);
            shader.setVec3("viewPos", camera.position);

            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
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

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_E && action == GLFW_PRESS)
    {

        if (camera.lookAtTarget)
        {
            camera.removeTarget();
        }
        else
        {
            camera.setTarget(mainCubePosition, glm::vec3(0, 0, 3));
        }
    }
}

void processInput(GLFWwindow *window, Camera &camera, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        if (camera.lookAtTarget)
        {
            mainCubePosition->z -= deltaTime * 1.0f;
        }
        else
        {
            camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        if (camera.lookAtTarget)
        {
            mainCubePosition->z += deltaTime * 1.0f;
        }
        else
        {
            camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        if (camera.lookAtTarget)
        {
            mainCubePosition->x -= deltaTime * 1.0f;
        }
        else
        {
            camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        if (camera.lookAtTarget)
        {
            mainCubePosition->x += deltaTime * 1.0f;
        }
        else
        {
            camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        if (camera.lookAtTarget)
        {
            mainCubePosition->y += deltaTime * 1.0f;
        }
        else
        {
            camera.ProcessKeyboard(Camera_Movement::UP, deltaTime);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        if (camera.lookAtTarget)
        {
            mainCubePosition->y -= deltaTime * 1.0f;
        }
        else
        {
            camera.ProcessKeyboard(Camera_Movement::DOWN, deltaTime);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        textureLevel += 1.5f * deltaTime;
        if (textureLevel >= 1.0f)
            textureLevel = 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        textureLevel -= 1.5f * deltaTime;
        if (textureLevel <= 0.0f)
            textureLevel = 0.0f;
    }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    SCR_WIDTH = (float)width;
    SCR_HEIGHT = (float)height;
    glViewport(0, 0, width, height);
}