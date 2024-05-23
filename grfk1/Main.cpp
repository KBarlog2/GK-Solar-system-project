#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "Shader.h"
#include "Camera.h"
#include "Object.h"
#include "Texture.h"
#define NUM_LIGHTS 6
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// Wielkosc okna
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 900;

// Kamera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Czas
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Funkcje
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// T³o
float backgroundVertices[] = {
    // positions   // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
};

unsigned int backgroundVAO, backgroundVBO;

int main() {
    //GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Tworzenie okna
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Projekt - Uklad sloneczny", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // shader t³a
    Shader backgroundShader("background_vertex_shader.glsl", "background_fragment_shader.glsl");

    // tekstura t³a
    Texture backgroundTexture("textures/bg.bmp");

    // setup VAO t³a
    glGenVertexArrays(1, &backgroundVAO);
    glGenBuffers(1, &backgroundVBO);
    glBindVertexArray(backgroundVAO);
    glBindBuffer(GL_ARRAY_BUFFER, backgroundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundVertices), backgroundVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // shadery
    Shader shader("vertex_shader.glsl", "fragment_shader.glsl");

    // Wczytywanie modeli planet
    Object sun;
    Texture sunTexture("textures/sun.bmp");
    Texture planetTextures[8] = {
    Texture("textures/mercury.bmp"),
    Texture("textures/venus.bmp"),
    Texture("textures/earth.bmp"),
    Texture("textures/mars.bmp"),
    Texture("textures/jupiter.bmp"),
    Texture("textures/saturn.bmp"),
    Texture("textures/uranus.bmp"),
    Texture("textures/neptun.bmp")
    };

    // Definicja planet
    Object planets[8];
    for (int i = 0; i < 8; ++i) {
        planets[i].loadModel("textures/planet.obj");
    }

    // Saturn ring
    Object saturnRing;
    saturnRing.setupRingMesh(1.2f, 2.0f, 64);
    Texture ringTexture("textures/saturn_ring.bmp");

    // G³ówna pêtla renderuj¹ca
    while (!glfwWindowShouldClose(window)) {
        // Czas pomiêdzy klatkami
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Obs³uga wejœcia
        processInput(window);

        // Renderowanie
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        // Renderowanie t³a
        backgroundShader.use();
        glBindVertexArray(backgroundVAO);
        backgroundTexture.bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // Enable depth testing
        glEnable(GL_DEPTH_TEST);


        glm::vec3 lightPositions[NUM_LIGHTS] = { 
            glm::vec3(2.0f, 0.0f, 0.0f),  // right
            glm::vec3(-2.0f, 0.0f, 0.0f), // left 
            glm::vec3(0.0f, 2.0f, 0.0f),  // above 
            glm::vec3(0.0f, -2.0f, 0.0f), // below 
            glm::vec3(0.0f, 0.0f, 2.0f),  // in front
            glm::vec3(0.0f, 0.0f, -2.0f)  // behind 
        };
        
        shader.use();
        for (unsigned int i = 0; i < NUM_LIGHTS; i++)
        {
            std::string lightPosName = "lightPos[" + std::to_string(i) + "]";
            shader.setVec3(lightPosName, lightPositions[i]);
        }
        shader.setVec3("viewPos", camera.Position);

        // Matryce przekszta³ceñ
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        // Render sun
        shader.setBool("isSun", true); 
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(1.0f)); // Sun is bigger
        shader.setMat4("model", model);
        sunTexture.bind();
        sun.draw(shader);



        shader.setBool("isSun", false); // Indicate that we are rendering planets
        for (int i = 0; i < 8; ++i) {
            // Nowy matrix dla kazdej planety
            glm::mat4 model = glm::mat4(1.0f);

            // Rotacja planet
            //float rotationSpeed = 0.015f * (i + 1); // Predkosc rotacji planet wokol slonca
            //float rotationSpeed = 0.0f;

            float rotationSpeed;
            switch (i) {
            case 0: rotationSpeed = 0.24f; break;
            case 1: rotationSpeed = 0.21f; break;
            case 2: rotationSpeed = 0.18f; break;
            case 3: rotationSpeed = 0.15f; break;
            case 4: rotationSpeed = 0.12f; break;
            case 5: rotationSpeed = 0.09f; break;
            case 6: rotationSpeed = 0.06f; break;
            case 7: rotationSpeed = 0.03f; break;
            default: rotationSpeed = 0.0f; break;
            }

            //float rotationSpeed = 0.0f * (i + 1); // 0 predkosc obrotu dla wygodniejszego widoku

            model = glm::rotate(model, (float)glfwGetTime() * rotationSpeed, glm::vec3(0.0f, 1.0f, 0.0f));

            // Odleglosc planet od slonca
            float distanceFromSun;
            switch (i) {
            case 0: distanceFromSun = 3.871f * 0.5f; break;
            case 1: distanceFromSun = 7.233f * 0.45f; break;
            case 2: distanceFromSun = 10.0f * 0.4f; break;
            case 3: distanceFromSun = 15.237f * 0.35f; break;
            case 4: distanceFromSun = 26.034f * 0.30f; break;
            case 5: distanceFromSun = 37.371f * 0.25f; break;
            case 6: distanceFromSun = 45.913f * 0.25f; break;
            case 7: distanceFromSun = 54.0f * 0.25f; break;
            default: distanceFromSun = 0.0f; break;
            }
            model = glm::translate(model, glm::vec3(distanceFromSun, 0.0f, 0.0f));

            // Rozmiar planet
            float planetSize;
            switch (i) {
            case 0: planetSize = 0.038f; break;
            case 1: planetSize = 0.09f; break;
            case 2: planetSize = 0.10f; break;
            case 3: planetSize = 0.05f; break;
            case 4: planetSize = 0.6f; break;
            case 5: planetSize = 0.45f; break;
            case 6: planetSize = 0.2f; break;
            case 7: planetSize = 0.19f; break;
            default: planetSize = 0.0f; break;
            }
            // Zmiana rozmiaru dla kazdej planety
            model = glm::scale(model, glm::vec3(planetSize));

            // Obrot planety wokol wlasnej osi
            float planetSpinSpeed = 1.0 * 2/(i + 1);

            model = glm::rotate(model, (float)glfwGetTime() * planetSpinSpeed, glm::vec3(0.1f, 1.0f, 0.1f));

            // Model matrix in shader
            shader.setMat4("model", model);

            // Bind tekstury
            planetTextures[i].bind();

            // Render planety
            planets[i].draw(shader);

            // Render Saturn's ring
            if (i == 5) { // Saturn is the sixth planet (index 5)
                glm::mat4 ringModel = glm::mat4(1.0f);
                ringModel = glm::translate(ringModel, glm::vec3(distanceFromSun, 0.0f, 0.0f));
                ringModel = glm::scale(ringModel, glm::vec3(planetSize));
                ringModel = glm::rotate(ringModel, (float)glfwGetTime() * planetSpinSpeed, glm::vec3(0.1f, 0.1f, 0.1f)); // Use the same rotation speed as the planet
                ringTexture.bind();
                saturnRing.drawRing(shader);
            }
        }


        // Disable depth testing after rendering planets
        glDisable(GL_DEPTH_TEST);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up resources
    glDeleteVertexArrays(1, &backgroundVAO);
    glDeleteBuffers(1, &backgroundVBO);
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
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

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}

void processInput(GLFWwindow* window) {
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
}
