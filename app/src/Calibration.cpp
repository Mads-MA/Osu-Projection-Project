#include <Calibration.hpp>

#include <Windows.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <WebcamCapture.hpp>
#include <WindowCapture.hpp>
#include <Window.hpp>

Calibration::Calibration(std::shared_ptr<WebcamCapture> webcamCapture, std::shared_ptr<WindowCapture> windowCapture)
    : webcamCapture{webcamCapture}, windowCapture{windowCapture}
{
    TryLoad();
}

void Calibration::ProcessInput() {
    auto ctx = glfwGetCurrentContext();
    static bool prevEscKeyState = false;
    bool curEscKeyState = glfwGetKey(ctx, GLFW_KEY_ESCAPE) == GLFW_PRESS;

    if (curEscKeyState && !prevEscKeyState) {
        windowEnabled = !windowEnabled;

        //Reset calibration if window is enabled
        if (windowEnabled == true) {
            currentState = State::Begin;
        }
    }

    prevEscKeyState = curEscKeyState;
}

void Calibration::Render() {
    if (!windowEnabled) {
        return;
    }

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("Calibration Window", nullptr, 
                 ImGuiWindowFlags_NoTitleBar | 
                 ImGuiWindowFlags_NoResize | 
                 ImGuiWindowFlags_NoMove | 
                 ImGuiWindowFlags_NoCollapse | 
                 ImGuiWindowFlags_NoBringToFrontOnFocus);

    switch (currentState) {
    case State::Begin:
        Begin();
        break;
    case State::ImageCapture:
        ImageCapture();
        break;
    case State::CorrespondenceSelection:
        CorrespondenceSelection();
        break;
    case State::Done:
        Done();
        break;
    }

    ImGui::End();
}

FrameData Calibration::ApplyProjection(FrameData &windowFrame, FrameData &webcamFrame) {
    cv::Mat windowMat (windowFrame.height, windowFrame.width, CV_8UC4, windowFrame.data.data());
    cv::Mat webcamMat (webcamFrame.height, webcamFrame.width, CV_8UC4, webcamFrame.data.data());
    
    cv::Mat webcamMatResized;
    // cv::resize(webcamMat, webcamMatResized, {3440, 1440});

    cv::Mat windowWarped;
    cv::flip(windowMat, windowMat, 0);
    cv::warpPerspective(windowMat, windowWarped, perspectiveTransform, {1920, 1080});

    cv::Mat blendedMat;
    double alpha = 0.4; //Higher is more webcam
    double beta = 0.6; //Higher is more window
    cv::addWeighted(webcamMat, alpha, windowWarped, beta, 0.0, blendedMat);

    FrameData blendedFrame;
    size_t frameSize = blendedMat.total() * blendedMat.elemSize();;
    blendedFrame.data.resize(frameSize); 
    std::memcpy(blendedFrame.data.data(), blendedMat.data, frameSize);
    blendedFrame.height = blendedMat.rows;
    blendedFrame.width = blendedMat.cols;
    blendedFrame.channels = blendedMat.channels();

    return blendedFrame;
}

void Calibration::TryLoad() {  
    cv::FileStorage fs(filename, cv::FileStorage::READ);
    if (!fs.isOpened()) {
        std::cout << "Could not find calibration file: " + filename << "\n";
        calibrated = false;
        return;
    }
    
    fs["perspectiveTransform"] >> perspectiveTransform;
    fs.release();
    
    if (perspectiveTransform.empty()) {
        std::cout << "Failed to load perspective transform from "  << filename << "\n";
    }

    calibrated = true;
    std::cout << "Loaded existing calibration." << std::endl;
}

void Calibration::Save() {
    cv::FileStorage fs(filename, cv::FileStorage::WRITE);
    if (!fs.isOpened()) {
        std::cout << "Could open calibration file " + filename << " to save.\n";
        return;
    }
    
    fs << "perspectiveTransform" << perspectiveTransform;
    fs.release();
    std::cout << "Calibration saved to " << filename << std::endl;
}

void Calibration::Reset() {
    correspondenceIndex = 0;
    correspondences.clear();
    windowTextures.clear();
    webcamTextures.clear();
    calibrated = false;
}

void Calibration::Begin() {
    //Unnecessary button styling
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    float buttonWidth = 200.0f;
    float buttonHeight = 50.0f;
    
    // Calculate position to center the button
    float posX = (displaySize.x - buttonWidth) * 0.5f;
    float posY = (displaySize.y - buttonHeight) * 0.5f;

    ImGui::SetCursorPos(ImVec2(posX, posY));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.3f, 1.0f));         
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.4f, 1.0f)); 
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.6f, 0.2f, 1.0f)); 
    float origScale = ImGui::GetFont()->Scale;
    ImGui::GetFont()->Scale = 1.5f; 
    ImGui::PushFont(ImGui::GetFont());
    
    if (ImGui::Button("Calibrate", ImVec2(buttonWidth, buttonHeight))) {
        currentState = State::ImageCapture;
        Reset();
    }
    
    ImGui::PopFont();
    ImGui::GetFont()->Scale = origScale;
    ImGui::PopStyleColor(3);
}

void Calibration::ImageCapture() {
    ImGui::TextWrapped("Open Osu!." "Hover the cursor near each corner of the application window." "For each corner press \"SPACE\" to capture an image for calibration.");
    
    // Check if Space key is currently pressed globally
    SHORT keyState = GetAsyncKeyState(VK_SPACE);
    if ((keyState & 0x0001) != 0) {
        std::cout << "Space was pressed. Cursor pos: " << windowCapture->GetCursorPosition() << "\n";
        Correspondence correspondence;
        correspondence.windowPos = windowCapture->GetCursorPosition();
        correspondence.webcam = webcamCapture->GetFrame();
        correspondence.window = windowCapture->GetFrame();
        correspondences.push_back(correspondence);
    }

    if (correspondences.size() >= 4) {
        
        for (const auto& correspondence : correspondences) {
            auto windowTex = std::make_shared<Texture>();
            windowTex->SetData(correspondence.window.data, 
                correspondence.window.width, 
                correspondence.window.height, 
                correspondence.window.channels);
                windowTextures.push_back(windowTex);
                
            auto webcamTex = std::make_shared<Texture>();
            webcamTex->SetData(correspondence.webcam.data, 
                correspondence.webcam.width, 
                correspondence.webcam.height, 
                correspondence.webcam.channels);
                webcamTextures.push_back(webcamTex);
        }
        currentState = State::CorrespondenceSelection;
    }
}

void Calibration::CorrespondenceSelection() {
    ImGui::TextWrapped("Click on the tablet pen position on the image shown");

    auto& currentCorr = correspondences[correspondenceIndex];
    
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 imgSize = ImVec2((float)currentCorr.webcam.width, (float)currentCorr.webcam.height);
    ImVec2 imagePos = ImGui::GetCursorScreenPos();
    ImTextureID textureID = webcamTextures[correspondenceIndex]->Get();

    ImGui::Image(textureID, imgSize);

    ImVec2 mousePos = io.MousePos;
    ImVec2 mousePosRelativeImage = {
        mousePos.x - imagePos.x,
        mousePos.y - imagePos.y
    };

    bool isMouseOverImage = 
                mousePosRelativeImage.x >= 0 && mousePosRelativeImage.x < imgSize.x &&
                mousePosRelativeImage.y >= 0 && mousePosRelativeImage.y < imgSize.y;

    if (io.MouseClicked[0] && isMouseOverImage) {
        currentCorr.webcamPos = {mousePosRelativeImage.x, mousePosRelativeImage.y};
        correspondenceIndex += 1;
    }

    //Once we have 4 corresponding points, we calculate the perspective transform
    if (correspondenceIndex == correspondences.size()) {
        cv::Point2f src[4];
        cv::Point2f dst[4];
        
        for (size_t i = 0; i < 4; i++)
        {
            auto& corr = correspondences[i];
            src[i] = cv::Point2f((float)corr.windowPos.x(), (float)corr.windowPos.y());
            dst[i] = cv::Point2f((float)corr.webcamPos.x(), (float)corr.webcamPos.y());
        }
        
        perspectiveTransform = cv::getPerspectiveTransform(src, dst);
    
        currentState = State::Done;
    }

    ImGui::Text("Mouse: (%f, %f)", mousePos.x, mousePos.y);
}

void Calibration::Done() {
    //Unnecessary styling
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    float buttonWidth = 200.0f;
    float buttonHeight = 50.0f;
    
    // Calculate position to center the text
    float posX = (displaySize.x - buttonWidth) * 0.5f;
    float posY = (displaySize.y - buttonHeight) * 0.5f;

    ImGui::SetCursorPos(ImVec2(posX, posY));
    ImGui::Text("Calibration done!");
    ImGui::Text("Press \"ESC\" to continue!");

    if (calibrated == false) {
        Save();
    }
    calibrated = true;
}