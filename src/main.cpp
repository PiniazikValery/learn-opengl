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
#include "./models/MinecraftCube/MinecraftCube.h"
#include "./TextureLoader/TextureLoader.h"
#include "./3DModel/Model/Model.h"

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
glm::vec3 modelPos(0.2f, 6.0f, 6.0f);
glm::vec3 sasukePos(-5.2f, 6.0f, 6.0f);
glm::vec3 narutoPos(-10.2f, 6.0f, 6.0f);

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
    Shader narutoCharacterShader("shaders/sasukeShaders/index.vs", "shaders/sasukeShaders/index.fs");
    Shader modelShader("shaders/backpackShaders/index.vs", "shaders/backpackShaders/index.fs");
    Shader shader("shaders/vertexShaderSource.vs", "shaders/fragmentShaderSource.fs");
    Shader lightShader("shaders/light/lightVertexShaderSource.vs", "shaders/light/lightFragmentShaderSource.fs");
    Shader lightingShader("shaders/light/lightingVertexShaderSource.vs", "shaders/light/lightingFragmentShaderSource.fs");

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

    unsigned int VBOs[2], VAOs[2], EBOs[2];
    glGenVertexArrays(2, VAOs); // мы также можем генерировать несколько VAO или буферов одновременно
    glGenBuffers(2, VBOs);
    glGenBuffers(2, EBOs);

    // Configuring cube vertices
    glBindVertexArray(VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
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

    // Configuring minecraft dirt cube vertices
    glBindVertexArray(VAOs[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(MinecraftCubeModel::indices), MinecraftCubeModel::indices, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(MinecraftCubeModel::vertices), MinecraftCubeModel::vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

    unsigned int firstTexture = TextureLoader::loadTexture("textures/wood_box.png");
    unsigned int secondTexture = TextureLoader::loadTexture("textures/marina.png");
    unsigned int specularTexture = TextureLoader::loadTexture("textures/wood_box_specular.png");
    unsigned int emissionTexture = TextureLoader::loadTexture("textures/box_emission.jpg");
    unsigned int minecraftDirtTexture = TextureLoader::loadTexture("textures/minecraft_dirt.png");

    //Load model
    Model ourModel("resources/objects/backpack/backpack.obj");
    Model sasukeModel("resources/objects/sasuke/sasuke.obj");
    Model narutoModel("resources/objects/naruto/naruto.obj");

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        // Delta time calculation
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, secondTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, firstTexture);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, specularTexture);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, emissionTexture);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, minecraftDirtTexture);

        processInput(window, camera, deltaTime);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        glm::mat3 normalMatrix = glm::mat3(1.0f);

        model = glm::translate(model, *mainCubePosition);
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));

        normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));

        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);

        shader.use();
        unsigned int modelLoc = glGetUniformLocation(shader.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        unsigned int viewLoc = glGetUniformLocation(shader.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));
        shader.setMat4("projection", projection);
        shader.setMat3("normalMatrix", normalMatrix);
        shader.setVec3("viewPos", camera.position);
        shader.setVec3("light.position", camera.position);
        shader.setVec3("light.direction", camera.front);
        shader.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
        shader.setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));
        shader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        shader.setVec3("light.diffuse", 1.0f, 1.0f, 1.0f);
        shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        shader.setFloat("light.constant", 1.0f);
        shader.setFloat("light.linear", 0.014f);
        shader.setFloat("light.quadratic", 0.0007f);
        shader.setInt("material.diffuse", 4);
        shader.setVec3("material.specular", 1.0f, 1.0f, 1.0f);
        shader.setFloat("material.shininess", 32.0f);

        glBindVertexArray(VAOs[1]);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        lightShader.use();
        model = glm::translate(glm::mat4(1.0f), lightPos);
        unsigned int lightModelLoc = glGetUniformLocation(lightShader.ID, "model");
        glUniformMatrix4fv(lightModelLoc, 1, GL_FALSE, glm::value_ptr(model));
        unsigned int lightViewLoc = glGetUniformLocation(lightShader.ID, "view");
        glUniformMatrix4fv(lightViewLoc, 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));
        lightShader.setMat4("projection", projection);
        glBindVertexArray(VAOs[0]);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        lightingShader.use();
        model = glm::translate(glm::mat4(1.0f), lightingPos);
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
        unsigned int lightingModelLoc = glGetUniformLocation(lightingShader.ID, "model");
        glUniformMatrix4fv(lightingModelLoc, 1, GL_FALSE, glm::value_ptr(model));
        unsigned int lightingViewLoc = glGetUniformLocation(lightingShader.ID, "view");
        glUniformMatrix4fv(lightingViewLoc, 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat3("normalMatrix", normalMatrix);
        lightingShader.setVec3("viewPos", camera.position);

        lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        lightingShader.setVec3("dirLight.ambient", 0.2f, 0.2f, 0.2f);
        lightingShader.setVec3("dirLight.diffuse", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);

        lightingShader.setInt("pointLightsCount", 1);
        lightingShader.setVec3("pointLights[0].position", lightPos);
        lightingShader.setVec3("pointLights[0].ambient", 0.2f, 0.2f, 0.2f);
        lightingShader.setVec3("pointLights[0].diffuse", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[0].constant", 1.0f);
        lightingShader.setFloat("pointLights[0].linear", 0.014f);
        lightingShader.setFloat("pointLights[0].quadratic", 0.0007f);

        lightingShader.setInt("spotLightsCount", 1);
        lightingShader.setVec3("spotLights[0].position", camera.position);
        lightingShader.setVec3("spotLights[0].direction", camera.front);
        lightingShader.setFloat("spotLights[0].cutOff", glm::cos(glm::radians(12.5f)));
        lightingShader.setFloat("spotLights[0].outerCutOff", glm::cos(glm::radians(17.5f)));
        lightingShader.setVec3("spotLights[0].ambient", 0.2f, 0.2f, 0.2f);
        lightingShader.setVec3("spotLights[0].diffuse", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("spotLights[0].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("spotLights[0].constant", 1.0f);
        lightingShader.setFloat("spotLights[0].linear", 0.014f);
        lightingShader.setFloat("spotLights[0].quadratic", 0.0007f);

        lightingShader.setInt("material.diffuse", 1);
        lightingShader.setInt("material.specular", 2);
        lightingShader.setFloat("material.shininess", 32.0f);
        glBindVertexArray(VAOs[0]);
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
            normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
            shader.use();
            shader.setInt("material.diffuse", 0);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            shader.setMat3("normalMatrix", normalMatrix);

            glBindVertexArray(VAOs[0]);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }

        modelShader.use();

        modelShader.setVec3("viewPos", camera.position);
        modelShader.setFloat("material.shininess", 32.0f);

        // Direction light
        modelShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        modelShader.setVec3("dirLight.ambient", 0.2f, 0.2f, 0.2f);
        modelShader.setVec3("dirLight.diffuse", 1.0f, 1.0f, 1.0f);
        modelShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);

        // Point light
        modelShader.setInt("pointLightsCount", 1);
        modelShader.setVec3("pointLights[0].position", lightPos);
        modelShader.setVec3("pointLights[0].ambient", 0.2f, 0.2f, 0.2f);
        modelShader.setVec3("pointLights[0].diffuse", 1.0f, 1.0f, 1.0f);
        modelShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        modelShader.setFloat("pointLights[0].constant", 1.0f);
        modelShader.setFloat("pointLights[0].linear", 0.014f);
        modelShader.setFloat("pointLights[0].quadratic", 0.0007f);

        // Spot light
        modelShader.setInt("spotLightsCount", 1);
        modelShader.setVec3("spotLights[0].position", camera.position);
        modelShader.setVec3("spotLights[0].direction", camera.front);
        modelShader.setFloat("spotLights[0].cutOff", glm::cos(glm::radians(12.5f)));
        modelShader.setFloat("spotLights[0].outerCutOff", glm::cos(glm::radians(17.5f)));
        modelShader.setVec3("spotLights[0].ambient", 0.2f, 0.2f, 0.2f);
        modelShader.setVec3("spotLights[0].diffuse", 1.0f, 1.0f, 1.0f);
        modelShader.setVec3("spotLights[0].specular", 1.0f, 1.0f, 1.0f);
        modelShader.setFloat("spotLights[0].constant", 1.0f);
        modelShader.setFloat("spotLights[0].linear", 0.014f);
        modelShader.setFloat("spotLights[0].quadratic", 0.0007f);

        modelShader.setMat4("projection", projection);
        unsigned int modelViewLoc = glGetUniformLocation(modelShader.ID, "view");
        glUniformMatrix4fv(modelViewLoc, 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));

        model = glm::mat4(1.0f);
        model = glm::translate(model, modelPos);
        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
        modelShader.setMat3("normalMatrix", normalMatrix);
        modelShader.setMat4("model", model);

        ourModel.Draw(modelShader);
        narutoCharacterShader.use();
        narutoCharacterShader.setVec3("viewPos", camera.position);
        narutoCharacterShader.setMat4("projection", projection);
        unsigned int sasukeViewLoc = glGetUniformLocation(narutoCharacterShader.ID, "view");
        glUniformMatrix4fv(sasukeViewLoc, 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));
        model = glm::mat4(1.0f);
        model = glm::translate(model, sasukePos);
        model = glm::scale(model, glm::vec3(100.0f, 100.0f, 100.0f));
        normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
        narutoCharacterShader.setMat3("normalMatrix", normalMatrix);
        narutoCharacterShader.setMat4("model", model);

        sasukeModel.Draw(narutoCharacterShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, narutoPos);
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
        narutoCharacterShader.setMat3("normalMatrix", normalMatrix);
        narutoCharacterShader.setMat4("model", model);

        narutoModel.Draw(narutoCharacterShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(2, VAOs);
    glDeleteBuffers(2, VBOs);

    glfwTerminate();
    return 0;
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }

    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos;
    lastX = (float)xpos;
    lastY = (float)ypos;

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
            mainCubePosition->z -= deltaTime * 5.0f;
            camera.followTarget();
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
            mainCubePosition->z += deltaTime * 5.0f;
            camera.followTarget();
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
            mainCubePosition->x -= deltaTime * 5.0f;
            camera.followTarget();
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
            mainCubePosition->x += deltaTime * 5.0f;
            camera.followTarget();
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
            mainCubePosition->y += deltaTime * 5.0f;
            camera.followTarget();
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
            mainCubePosition->y -= deltaTime * 5.0f;
            camera.followTarget();
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
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    glViewport(0, 0, width, height);
}