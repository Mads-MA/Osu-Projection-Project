#ifndef CALIBRATION_HPP
#define CALIBRATION_CPP

#include <Eigen/Eigen>
#include <opencv2/opencv.hpp>
#include <vector>
#include <memory>
#include <utility>

#include <FrameData.hpp>
#include <WebcamCapture.hpp>
#include <WindowCapture.hpp>

#include <Texture.hpp>

struct Correspondence {
    FrameData window;
    FrameData webcam;
    Eigen::Vector2i windowPos;
    Eigen::Vector2i webcamPos;
};

class Calibration {
public:
    enum class State : uint8_t {
        Begin,
        ImageCapture,
        CorrespondenceSelection,
        Done,
    };

    Calibration(std::shared_ptr<WebcamCapture> webcamCapture, std::shared_ptr<WindowCapture> windowCapture);

    void ProcessInput();
    void Render();
    FrameData ApplyProjection(FrameData& windowFrame, FrameData& webcamFrame);
    inline bool Calibrated() const { return calibrated; }
private:
    void TryLoad();
    void Save();
    void Reset();
    void Begin();
    void ImageCapture();
    void CorrespondenceSelection();
    void Done();

private:
    State currentState = State::Begin;
    bool windowEnabled = false;

    size_t correspondenceIndex = 0;
    std::vector<Correspondence> correspondences;
    std::vector<std::shared_ptr<Texture>> windowTextures;
    std::vector<std::shared_ptr<Texture>> webcamTextures;


    std::shared_ptr<WebcamCapture> webcamCapture; 
    std::shared_ptr<WindowCapture> windowCapture;

    bool calibrated = false;
    cv::Mat perspectiveTransform;
    const std::string filename = "calibration.yml";
};

#endif