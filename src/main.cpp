#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "FilepathManager.hpp"

#include "OpenGLManager.hpp"
#include "shapes.hpp"
#include "InputManager.hpp"
#include <vector>

#include "ModelManager.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_opengl3.h"

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create an SDL window
    SDL_Window* window = SDL_CreateWindow("OpenGL", 100, 100, 1600, 1200, SDL_WINDOW_OPENGL );
    if (window == nullptr) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }
    //SDL_SetRelativeMouseMode(SDL_TRUE);

    // Create an OpenGL context
    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (context == nullptr) {
        std::cerr << "SDL_GL_CreateContext Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    FilepathManager* myFilepathManager = new FilepathManager();

    OpenGLManager myOpenGLManager(myFilepathManager);

    // Create shapes
    glm::vec3 red(1.0f, 0.0f, 0.0f);
    glm::vec3 green(0.0f, 1.0f, 0.0f);
    glm::vec3 blue(0.0f, 0.0f, 1.0f);
    glm::vec3 purple(0.627f, 0.125f, 0.941f);

    glm::vec3 left(-2.0f, 1.0f, 0.0f);
    glm::vec3 middle(0.0f, 1.0f, 0.0f);
    glm::vec3 right(2.0f, 1.0f, 0.0f);

    std::vector<Shape*> shapes;
    shapes.push_back(new Square(red, left));
    shapes.push_back(new Square(green, middle));
    shapes.push_back(new Square(blue, right));
    shapes.push_back(new Plane(100.0f, 100, purple));

    for (Shape* shape : shapes) {
        shape->initialize();
    }

    myOpenGLManager.setupLighting();

    InputManager myInputManager(&myOpenGLManager, &shapes);

    ////
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable keyboard navigation (optional)
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable gamepad navigation (optional)

    ImGui::StyleColorsLight(); // You can choose Light, Classic, or custom styles.

    ImGui_ImplSDL2_InitForOpenGL(window, context); // Replace with your SDL window and OpenGL context.
    ImGui_ImplOpenGL3_Init("#version 330 core"); // Replace `#version 330 core` with your GLSL version.
    ////

    size_t hash_key_default_scene = std::hash<std::string>{}("Default Scene");

    // Main loop
    bool running = true;
    SDL_Event event;
    float angle = 0.0f;
    while (running) {
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) {
                running = false;
            }
            myInputManager.handleKeys(event);
        }

        myInputManager.updateCamera();
        if (myOpenGLManager.cameraPos.y < 1.0f) {
            myOpenGLManager.cameraPos.y = 1.0f;
        }
        running = myInputManager.processInput();
        myInputManager.highlightIntersectedObject();

        angle += 0.01f;

        // Render
        myOpenGLManager.ClearScreen();
        myOpenGLManager.UpdateViewAndProjectionMatrices();

        for (Shape* shape : shapes) {
            shape->render(myOpenGLManager.shaderProgram, myOpenGLManager.view, myOpenGLManager.proj, myOpenGLManager.cameraPos, angle);
        }

        myOpenGLManager.doPrototypeDrawCall(angle*10);
        myOpenGLManager.doDrawScene(hash_key_default_scene);
        /*myModelManager.drawModel(myOpenGLManager.shaderProgram, angle*10);
        myModelManager.drawModelLoaded(myOpenGLManager.shaderProgram);
        myModelManager.drawModelFromHash(myOpenGLManager.shaderProgram, hash_key_evergreen_tree);*/
        //myModelManager.drawModelFromHash(myOpenGLManager.shaderProgram, evergreen_tree_textured);
        //myModelManager.drawModelFromHash(myOpenGLManager.shaderProgram, barramundi);

        /////
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();
        
        ImGui::Begin("OpenGlViewer");
        ImGui::Text("by Abraham Arvedon");
        
        if (ImGui::Button("Load Model")) {
            myOpenGLManager.loadModelButton();
        }

        ImGui::End();

        myOpenGLManager.renderShaderMenu();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        /////


        // Swap buffers
        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    glDeleteProgram(myOpenGLManager.shaderProgram);
    glDeleteBuffers(1, &myOpenGLManager.VBO);
    glDeleteVertexArrays(1, &myOpenGLManager.VAO);

    //ImGUI Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
