#include <string>
#include <thread>

#include <RenderTexture.hpp>
#include <WindowCapture.hpp>
#include <WebcamCapture.hpp>
#include <Window.hpp>
#include <Calibration.hpp>



int main() {
    Window window("Osu! Projection Project", 1980, 1080);
    RenderTexture screenTexture;
    std::shared_ptr<WindowCapture> windowCap = std::make_shared<WindowCapture>("osu!");
    windowCap->Start();

    std::shared_ptr<WebcamCapture> camCap = std::make_shared<WebcamCapture>(0);
    camCap->Start();

    Calibration calibration(camCap, windowCap);

    while (!window.ShouldClose()) {
        window.ProcessInput();
        calibration.ProcessInput();

        window.PrepareFrame();
        
        calibration.Render();


        auto camFrame = camCap->GetFrame();
        auto windowFrame = windowCap->GetFrame();

        if (calibration.Calibrated()) {
            FrameData blendedFrame = calibration.ApplyProjection(windowFrame, camFrame);
            screenTexture.SetTexture(blendedFrame.data, blendedFrame.width, blendedFrame.height, blendedFrame.channels);
        }

        screenTexture.Render();

        window.SubmitFrame();
    }
    return 0;
}