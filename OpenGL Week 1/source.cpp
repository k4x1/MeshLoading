/*
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) 2024 Media Design School
File Name : source.cpp
Description : Main entry point for the OpenGL application. Initializes GLFW, GLEW, sets up the main loop, all models, etc.
Author : Kazuo Reis de Andrade
Mail : kazuo.andrade@mds.ac.nz
*/
#include <memory>
#include "GameScene.h"


// Function prototypes
std::unique_ptr<Scene> CurrentScene;


int main()
{
    CurrentScene = std::make_unique<GameScene>();


    // Run the main loop
    CurrentScene->MainLoop();
    return 0;
}
