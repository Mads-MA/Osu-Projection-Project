#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <memory>


class Window {
public:
    Window(std::string name_, int width_, int height_);
    ~Window();
    void PrepareFrame();
    void ProcessInput();
    void SubmitFrame();
    bool ShouldClose();
    GLFWwindow* Ctx() { return context; }
private:
    static void OnResize(GLFWwindow* context_, int width_, int height_);
private:
    
    int width, height;
    std::string name;

    GLFWwindow* context;
};

#endif 