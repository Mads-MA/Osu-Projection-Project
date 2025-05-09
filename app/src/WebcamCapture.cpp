#include <WebcamCapture.hpp>

WebcamCapture::WebcamCapture(int camIndex) {
    apiPref = cv::CAP_ANY;
    cameraIndex = camIndex;
}

WebcamCapture::~WebcamCapture() {
    Stop();
}


void WebcamCapture::Start() {
    capDevice.open(cameraIndex, apiPref);
    capDevice.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);
    capDevice.set(cv::CAP_PROP_FRAME_WIDTH, 1920);
}

void WebcamCapture::Stop() {
    capDevice.release();
}

FrameData WebcamCapture::GetFrame() {
    cv::Mat frame; 
    if (!capDevice.isOpened()) {
        Start();
    }

    capDevice >> frame;

    if (frame.empty()) {
        return lastFrame;
    }

    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGBA);
    cv::flip(frame, frame, 0);

    size_t frameSizeBytes = frame.total() * frame.elemSize();
    if (lastFrame.data.size() !=  frameSizeBytes) {
        lastFrame.data.resize(frame.total() * frame.elemSize(), 0);
    }

    std::memcpy(lastFrame.data.data(), frame.data, frameSizeBytes);
    lastFrame.width = frame.cols;
    lastFrame.height = frame.rows;
    lastFrame.channels = frame.channels();

    return lastFrame;
}