#ifndef WINDOW_CAPTURE_HPP
#define WINDOW_CAPTURE_HPP

#include <string>
#include <Windows.h>
#include <winrt/base.h>
#include <windows.graphics.capture.interop.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Metadata.h>
#include <winrt/Windows.Graphics.Capture.h>
#include <winrt/Windows.Graphics.DirectX.h>
#include <winrt/Windows.Graphics.DirectX.Direct3D11.h>
#include <windows.graphics.directx.direct3d11.interop.h>
#include <windows.graphics.capture.interop.h>
#include <d3d11.h>
#include <wrl.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <Eigen/Eigen>
#include <FrameData.hpp>

// Initialize Windows Runtime
#pragma comment(lib, "windowsapp")


class WindowCapture {
public:
    WindowCapture(std::string windowName);
    ~WindowCapture();
    void Start();
    void Stop();
    FrameData GetFrame();
    bool WindowActive();
    Eigen::Vector2i GetCursorPosition();
    FrameData LastFrame();

private:

private:
    HWND handle;
    Microsoft::WRL::ComPtr<ID3D11Device> d3dDevice;
    Microsoft::WRL::ComPtr<ID3D11Device> d3dDevice2;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> d3dContext;

    winrt::Windows::Graphics::Capture::GraphicsCaptureItem captureItem = {nullptr};
    winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool framePool = {nullptr};
    winrt::Windows::Graphics::Capture::GraphicsCaptureSession captureSession = {nullptr};

    RECT windowRect;
    int windowWidth = 0;
    int windowHeight = 0;

    FrameData lastFrame;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> cpuTexture;
};

#endif