#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Core/Graphics/Renderer.h>
#include <Core/CameraController.h>
#include <Core/Input/InputControl.h>
#include <Core/UI/GUIManager.h>
#include <Core/Textures/CubeTexture.h>
#include <memory>

class SHLighting {
public:
    static SHLighting& GetInstance();
    ~SHLighting();


    void Init();
    void Run();
    std::shared_ptr<InputContorl> GetInputController() const { return _input; }
    int GetHeight() const { return _height; }
    int GetWidth() const { return _width; }

private:
    // 构造函数private声明，确保单例
    SHLighting(int width, int height);

    int _width, _height;
    double _frameTime;
    GLFWwindow* _window;

    std::shared_ptr<Renderer> _renderer;
    std::shared_ptr<CameraController> _cameraControl;
    std::shared_ptr<InputContorl> _input;
    std::shared_ptr<GUIManager> _guiManager;
    

    std::shared_ptr<CubeTexture> _cubeTexture;

    std::vector<DrawTriangle> _drawTriangles;

    void update();
    void draw();
};
