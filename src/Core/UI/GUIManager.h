#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

class GUIManager {
public:
    GUIManager(GLFWwindow* window) : _window(window) { _elapsedTime = 1; }
    ~GUIManager();

    void Init();

    void Begin();
    void End();

    void SetElapsedTime(double t) { _elapsedTime = t; }

private:
    GLFWwindow* _window;
    double _elapsedTime;
};