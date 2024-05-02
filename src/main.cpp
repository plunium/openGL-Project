#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <memory>
#include <regex>
#include <algorithm>

#include "shader.hpp"
#include "constants.hpp"
#include "color.hpp"
#include "camera.hpp"
#include "gameObject.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Caméra
Camera camera(CAMERA_START_POSITION);

// Shader pour les objets, initialisé plus tard dans le main()
Shader objectShader;

// Temps pour une itération de la boucle de rendu
float deltaTime = 0.0f;
// Temps du dernier appel de la boucle de rendu
float lastFrame = 0.0f;

// Variables pour la gestion de la souris
bool firstMouse = true;
float lastX;
float lastY;

std::vector<std::unique_ptr<GameObject>> gameObjects;

// Variables pour la gestion du clavier
bool graveAccentKeyPressed = false;

// Variables pour faire apparaître la souris
bool mouseHidden = true;

// Définition du motif regex pour les instructions de création d'un gameObject
std::regex gameObjectCreationPattern(R"(^(\S+)\s+(\S+)\s+(0|1)$)");

// Définition des motifs regex pour les instructions de transformation d'un gameObject
std::regex translatePattern(R"(t\s+([-\d\.]+)\s+([-\d\.]+)\s+([-\d\.]+))");
std::regex rotatePattern(R"(r\s+([-\d\.]+)\s+([-\d\.]+)\s+([-\d\.]+)\s+([-\d\.]+))");
std::regex scalePattern(R"(s\s+([-\d\.]+)\s+([-\d\.]+)\s+([-\d\.]+))");

// Fonction appelée lors du redimensionnement de la fenêtre
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void applyTransformations(const std::string &input)
{
    // Extraire le nom du GameObject au début de l'entrée
    size_t firstSpace = input.find(' ');
    if (firstSpace == std::string::npos)
    {
        std::cout << "Format d'entree invalide." << std::endl;
        return;
    }
    std::string gameObjectName = input.substr(0, firstSpace);
    std::string transformations = input.substr(firstSpace + 1);

    // Trouver le GameObject par son nom
    auto it = std::find_if(gameObjects.begin(), gameObjects.end(), [&gameObjectName](const std::unique_ptr<GameObject> &obj)
                           { return obj->getName() == gameObjectName; });

    if (it == gameObjects.end())
    {
        std::cout << "GameObject '" << gameObjectName << "' non trouve." << std::endl;
        return;
    }
    auto &gameObject = *it; // Référence au GameObject trouvé

    bool transformationApplied = false; // Pour vérifier si une transformation a été appliquée

    // Fonction pour appliquer les transformations
    auto apply = [&](const std::regex &pattern, const std::string &transformationInput, auto transformFunc)
    {
        std::smatch matches;
        std::string::const_iterator searchStart = transformationInput.cbegin();
        while (std::regex_search(searchStart, transformationInput.cend(), matches, pattern))
        {
            transformationApplied = true;
            transformFunc(matches);
            searchStart = matches.suffix().first; // Continue à chercher après la dernière correspondance
        }
    };

    // Application des translations
    apply(translatePattern, transformations, [&](const std::smatch &matches)
          {
        float x = std::stof(matches[1].str()), y = std::stof(matches[2].str()), z = std::stof(matches[3].str());
        gameObject->modelMatrixTranslate(glm::vec3(x, y, z));
        std::cout << "Translation appliquee: x=" << x << ", y=" << y << ", z=" << z << std::endl; });

    // Application des rotations
    apply(rotatePattern, transformations, [&](const std::smatch &matches)
          {
        float angle = std::stof(matches[1].str()), x = std::stof(matches[2].str()), y = std::stof(matches[3].str()), z = std::stof(matches[4].str());
        gameObject->modelMatrixRotate(angle, glm::vec3(x, y, z));
        std::cout << "Rotation appliquee: angle=" << angle << ", x=" << x << ", y=" << y << ", z=" << z << std::endl; });

    // Application des mises à l'échelle
    apply(scalePattern, transformations, [&](const std::smatch &matches)
          {
        float x = std::stof(matches[1].str()), y = std::stof(matches[2].str()), z = std::stof(matches[3].str());
        gameObject->modelMatrixScale(glm::vec3(x, y, z));
        std::cout << "Mise a l'echelle appliquee: x=" << x << ", y=" << y << ", z=" << z << std::endl; });

    if (!transformationApplied)
    {
        std::cout << "Aucune transformation appliquee." << std::endl;
    }
}

// Fonction appelée lors de l'appui sur une touche du clavier
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Gestion des déplacements de la caméra avec les touches du clavier
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        camera.processKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
        camera.processKeyboard(DOWN, deltaTime);

    // Montrer/Masquer curseur souris
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        if(mouseHidden)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            mouseHidden = false;
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            mouseHidden = true;
        }
    }

    // Menu "²"
    if (glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT) == GLFW_PRESS && !graveAccentKeyPressed)
    {
        graveAccentKeyPressed = true;
        // Menu principal
        std::cout << "Menu principal :"
                  << std::endl
                  << "Entrez 1 pour creer un nouveau GameObject."
                  << std::endl
                  << "Entrez 2 pour appliquer une transformation a un GameObject existant."
                  << std::endl;

        std::string choice;
        std::getline(std::cin, choice);

        if (choice == "1" || choice == "2")
        {
            // On convertit le choix en entier
            int choiceInt = std::stoi(choice);

            // Création d'un GameObject
            if (choiceInt == 1)
            {
                std::cout << "Pour creer un nouveau GameObject :"
                          << std::endl
                          << "nomDuGameObject path/vers/mon/modele.obj 1 pour inverser verticalement les texture ou 0 pour ne pas les inverser :"
                          << std::endl;

                std::string userInput;
                std::getline(std::cin, userInput);
                std::smatch matches;
                if (std::regex_search(userInput, matches, gameObjectCreationPattern))
                {
                    std::string gameObjectName = matches[1];
                    std::string objectPath = matches[2];
                    bool flipTextureVertically = matches[3] == "1";

                    std::cout << "GameObject '" << gameObjectName << "' cree.\n"
                              << "Path: " << objectPath << "\n"
                              << "Inverser verticalement les textures: " << std::boolalpha << flipTextureVertically << std::endl;

                    gameObjects.push_back(std::make_unique<GameObject>(gameObjectName, objectPath, flipTextureVertically, objectShader, gameObjects));
                }
                else
                {
                    std::cout << "Format d'entree invalide." << std::endl;
                }
            }
            // Modification de GameObject
            else if (choiceInt == 2)
            {
                std::cout << "Pour modifier un gameObject :"
                          << std::endl
                          << "nomDuGameObject t valeurX valeurY valeurZ r valeurAngle valeurAxeX valeurAxeY valeurAxeZ s valeurX valeurY valeurZ :"
                          << std::endl;
                string userInput;
                std::getline(std::cin, userInput);
                applyTransformations(userInput);
            }
        }
        else
        {
            std::cout << "Entree invalide." << std::endl;
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT) == GLFW_RELEASE)
    {
        graveAccentKeyPressed = false;
    }
}

// Fonction appelée lors du déplacement de la souris
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    // On convertit les coordonnées de la souris en float
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    // Si c'est la première fois qu'on appelle la fonction, on initialise les coordonnées de la souris
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    // On calcule le déplacement de la souris depuis le dernier appel de la fonction
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Inversé car les coordonnées vont de bas en haut
    lastX = xpos;
    lastY = ypos;

    camera.processMouseMovement(xoffset, yoffset);
}

// Fonction appelée lors du défilement de la molette de la souris
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.processMouseScroll(static_cast<float>(yoffset));
}

// Initialisation de Dear ImGui
void initImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui::StyleColorsDark();
}

// Nettoyage de Dear ImGui
void cleanupImGui() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

// Rendu de Dear ImGui
void renderImGui() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Code de l'interface utilisateur
    static int numPointLights = 0;
    if (ImGui::Begin("Point Lights")) {
        if (ImGui::Button("Add Point Light")) {
            numPointLights++;
        }
        // ...
        ImGui::End();
    }

    // Rendu des points lumineux
    for (int i = 0; i < numPointLights; i++) {
        // Définition des propriétés du point lumineux
        GLfloat lightPosition[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        GLfloat lightAmbient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
        GLfloat lightDiffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
        GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        GLfloat lightConstant[] = { 1.0f, 0.0f, 0.0f, 1.0f };
        GLfloat lightLinear[] = { 0.0f, 1.0f, 0.0f, 1.0f };
        GLfloat lightQuadratic[] = { 0.0f, 0.0f, 1.0f, 1.0f };

        // Activation du point lumineux
        glEnable(GL_LIGHT0 + i);
        glLightfv(GL_LIGHT0 + i, GL_POSITION, lightPosition);
        glLightfv(GL_LIGHT0 + i, GL_AMBIENT, lightAmbient);
        glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, lightDiffuse);
        glLightfv(GL_LIGHT0 + i, GL_SPECULAR, lightSpecular);
        glLightfv(GL_LIGHT0 + i, GL_CONSTANT_ATTENUATION, lightConstant);
        glLightfv(GL_LIGHT0 + i, GL_LINEAR_ATTENUATION, lightLinear);
        glLightfv(GL_LIGHT0 + i, GL_QUADRATIC_ATTENUATION, lightQuadratic);

        // Mise à jour de la position des points lumineux
        GLfloat lightPosition[] = { cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0f };
        for (int i = 0; i < numPointLights; i++) {
            glLightfv(GL_LIGHT0 + i, GL_POSITION, lightPosition);
        }
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

int main()
{
    // Initialisation de GLFW
    glfwInit();
    // On dit à GLFW qu'on veut utiliser OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // On dit à GLFW qu'on veut utiliser le core profile qui ne fournit que les fonctionnalités modernes d'OpenGL
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Création de la fenêtre GLFW
    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    // On rend le contexte de la fenêtre actif pour les appels OpenGL
    glfwMakeContextCurrent(window);

    // Initialisation de GLAD, ce qui configure les pointeurs de fonctions OpenGL pour qu'on puisse les utiliser
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // On dit à OpenGL la taille de la fenêtre pour le viewport
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    // On appelle la fonction framebuffer_size_callback à chaque fois que la fenêtre est redimensionnée
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    objectShader = Shader(OBJECT_VERTEX_SHADER_PATH, OBJECT_FRAGMENT_SHADER_PATH);
    Shader lightSourceShader(LIGHT_VERTEX_SHADER_PATH, LIGHT_FRAGMENT_SHADER_PATH);

    // Initialisation de Dear ImGui
    initImGui();

    // Boucle de rendu
    while (!glfwWindowShouldClose(window))
    {
        // On calcule le temps écoulé depuis le dernier appel de la boucle de rendu
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // On traite un éventuel appui sur une touche (ici, ECHAP pour fermer la fenêtre)
        processInput(window);

        // On nettoie la couleur du buffer d'écran et on la remplit avec la couleur de fond
        glClearColor(CLEAR_COLOR.r, CLEAR_COLOR.g, CLEAR_COLOR.b, CLEAR_COLOR.a);
        // On nettoie le buffer de couleur et on le remplit avec la couleur précédemment définie
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Rendu de Dear ImGui
        renderImGui();

        // On échange les buffers de la fenêtre pour que ce qu'on vient de dessiner soit visible
        glfwSwapBuffers(window);
        // On regarde s'il y a des évènements (appui sur une touche, déplacement de la souris, etc.)
        glfwPollEvents();
    }

    // Nettoyage de Dear ImGui
    cleanupImGui();

    // Quand la fenêtre est fermée, on libère les ressources
    glDeleteVertexArrays(1, &lightSourceVAO);
    glDeleteBuffers(1, &VBO);
    gameObjects.clear();
    objectShader.deleteProgram();
    lightSourceShader.deleteProgram();

    // On termine GLFW
    glfwTerminate();
    return 0;
}