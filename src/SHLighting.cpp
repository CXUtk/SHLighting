#include "SHLighting.h"
#include <Core/Loaders/ObjLoader.h>

SHLighting& SHLighting::GetInstance() {
    static SHLighting game(800, 600);
    return game;
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

SHLighting::~SHLighting() {
    glfwDestroyWindow(_window);
    glfwTerminate();
}

void SHLighting::Init() {
    _renderer = std::make_shared<Renderer>();

    _input = std::make_shared<InputContorl>(_window);
    _cameraControl = std::make_shared<CameraController>(glm::vec3(0, 0, 2));

    _guiManager = std::make_shared<GUIManager>(_window);
    _guiManager->Init();


    ObjLoader loader;
    loader.load("Resources/Scenes/spot_triangulated_good.obj");
    auto mesh = loader.GetMesh();
    _drawTriangles = mesh.GetDrawTriangles();

    loader.load("Resources/Scenes/cube.obj");
    mesh = loader.GetMesh();
    _drawTriangles2 = mesh.GetDrawTriangles();

    std::vector<std::string> faces
    {
        "Resources/Textures/right.jpg",
        "Resources/Textures/left.jpg",
        "Resources/Textures/top.jpg",
        "Resources/Textures/bottom.jpg",
        "Resources/Textures/front.jpg",
         "Resources/Textures/back.jpg"
    };
    _cubeTexture = std::make_shared<CubeTexture>(faces);
}


static double oldTime;
void SHLighting::Run() {
    oldTime = glfwGetTime();
    while (!glfwWindowShouldClose(_window)) {
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        update();
        draw();

        glfwSwapBuffers(_window);
        glfwPollEvents();

        double curTime = glfwGetTime();
        _frameTime = curTime - oldTime;
        oldTime = curTime;
    }
}

SHLighting::SHLighting(int width, int height) :_width(width), _height(height), _frameTime(1.0) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    _window = glfwCreateWindow(width, height, "3D Mesh Editor", nullptr, nullptr);
    if (!_window) {
        fprintf(stderr, "Failed to create window\n");
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(_window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to load glad!\n");
        glfwTerminate();
        return;
    }
    glfwSetFramebufferSizeCallback(_window, framebuffer_size_callback);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
}

void SHLighting::update() {
    _guiManager->Begin();
    _input->beginInput();

    _cameraControl->Run();
}

void SHLighting::draw() {
    _renderer->Begin(_cameraControl->GetCamera()->GetProjectTransform(), _cameraControl->GetCamera()->GetViewTransform());
    {
        _renderer->DrawSkyBox(_drawTriangles2, _cubeTexture);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);

        glDepthFunc(GL_LESS);
        auto dir = glm::normalize(glm::vec3(1, 1, 0));
        // _renderer->DrawLightedTriangles(_drawTriangles, glm::vec3(1), dir, _cameraControl->GetCamera()->GetEyePos());
        // _renderer->DrawTrianglesCubemap(_drawTriangles, _cubeTexture, _cameraControl->GetCamera()->GetEyePos());
        _renderer->DrawTrianglesSH(_drawTriangles);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        //_renderer->drawLines(tEdges, glm::vec3(1, 0, 0), 2);
    }
    _renderer->End();

    _input->endInput();
    _guiManager->End();
}
