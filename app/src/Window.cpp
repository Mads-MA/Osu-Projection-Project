#include <Window.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>
#include <cassert>
#include <functional>

static void glfw_error_callback(int error, const char* description)
{
    std::cerr << "GLFW Error, code: " << error << "\nDescription: " << description << "\n";
}

void Window::PrepareFrame() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Window::ProcessInput() {
}

void Window::SubmitFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(context);
    glfwPollEvents();
}

bool Window::ShouldClose() {
    return glfwWindowShouldClose(context);
}

Window::Window(std::string name_, int width_, int height_) {
    name = name_;
    width = width_;
    height = height_;

	if (!glfwInit()) {
		assert((false) && "Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	context = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
	if (context == nullptr) {
		assert((false) && "Failed to create GLFW window");
	}	
	glfwMakeContextCurrent(context);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		assert((false) && "Failed to initialize GLAD");
	}

	glViewport(0, 0, width, height);
	// glEnable(GL_DEPTH_TEST);

    glfwSetWindowUserPointer(context, this);
	glfwSetFramebufferSizeCallback(context, &Window::OnResize);


    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(context, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

Window::~Window() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(context);
    glfwTerminate();

}

void Window::OnResize(GLFWwindow* window, int width_, int height_) {
	glViewport(0, 0, width_, height_);
}