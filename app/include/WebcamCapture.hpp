#ifndef WEBCAM_CAPTURE_HPP
#define WEBCAM_CAPTURE_HPP

#include <opencv2/opencv.hpp>
#include <FrameData.hpp>
#include <string>

class WebcamCapture {
public:
    WebcamCapture(int camIndex = 0);
    ~WebcamCapture();

    void Start();
    void Stop();

    FrameData GetFrame();

private:
    void SetResolution(int width, int height);
    
private:
    cv::VideoCapture capDevice;
    int cameraIndex;
    int apiPref;
    FrameData lastFrame; 
};

#endif