#include <WindowCapture.hpp>
#include <exception>
#include <thread>


using Microsoft::WRL::ComPtr;

using namespace winrt;
using namespace winrt::Windows::Graphics::Capture;
using namespace winrt::Windows::Graphics::DirectX;
using namespace winrt::Windows::Graphics::DirectX::Direct3D11;
using namespace Microsoft::WRL;


// Helper to convert ID3D11Device to IDirect3DDevice
winrt::com_ptr<::IInspectable> CreateDirect3DDevice(ComPtr<ID3D11Device> const& d3dDevice) {
    ComPtr<::IDXGIDevice> dxgiDevice;
    d3dDevice.As(&dxgiDevice);  // WRL ComPtr uses As(&target)
    
    winrt::com_ptr<::IInspectable> device;
    winrt::check_hresult(CreateDirect3D11DeviceFromDXGIDevice(
        dxgiDevice.Get(), device.put()));
    
    return device;
}

// Helper to get the D3D11 device from a IDirect3DDevice
ComPtr<ID3D11Device> GetD3DDeviceFromDirect3DDevice(IDirect3DDevice const& device) {
    auto dxgiInterfaceAccess = device.as<::Windows::Graphics::DirectX::Direct3D11::IDirect3DDxgiInterfaceAccess>();
    ComPtr<ID3D11Device> d3dDevice;
    winrt::check_hresult(dxgiInterfaceAccess->GetInterface(IID_PPV_ARGS(&d3dDevice)));
    return d3dDevice;
}

// Helper function to get DXGI interface from DirectX surface
template <typename T>
HRESULT GetDXGIInterface(
    winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DSurface const& surface,
    _COM_Outptr_ T** dxgiInterface)
{
    auto access = surface.as<::Windows::Graphics::DirectX::Direct3D11::IDirect3DDxgiInterfaceAccess>();
    return access->GetInterface(IID_PPV_ARGS(dxgiInterface));
}


WindowCapture::WindowCapture(std::string windowName) {
    winrt::init_apartment(); // Initialize Windows Runtime

    //Create 3D direct11 device
    D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
    UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

    HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 
                                    flags, featureLevels, ARRAYSIZE(featureLevels),
                                    D3D11_SDK_VERSION, &d3dDevice, nullptr, &d3dContext);
    if (FAILED(hr)) {
        std::cerr << "Failed to create D3D11 device: " << std::hex << hr << std::endl;
        throw std::runtime_error("Failed to create D3D11 device");
    }

    // Create DirectX device for WinRT
    winrt::com_ptr<::IInspectable> inspectable = CreateDirect3DDevice(d3dDevice);
    IDirect3DDevice device = inspectable.as<IDirect3DDevice>();
    d3dDevice2 = GetD3DDeviceFromDirect3DDevice(device);

    //Find window and bring to front before getting window size.
    handle = FindWindowA(NULL, windowName.c_str());
    if (!handle) {
        std::cerr << "Could not find " << windowName << ". Make sure it is running" << std::endl;
        throw std::runtime_error("Failed to find specified window: " + windowName);
    }

    if (IsIconic(handle)) {
        ShowWindow(handle, SW_RESTORE);
    }

    ShowWindow(handle, SW_SHOW);
    if (!SetForegroundWindow(handle)) {
        std::cerr << "Could not set " << windowName << " to foreground" << std::endl;
    }

    // Get window size
    while (windowWidth == 0 && windowHeight == 0) {

        GetClientRect(handle, &windowRect);
        windowWidth = windowRect.right - windowRect.left;
        windowHeight = windowRect.bottom - windowRect.top;
        std::cout << "window size: " << windowWidth << "x" << windowHeight << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Create capture item
    auto interop = winrt::get_activation_factory<GraphicsCaptureItem, IGraphicsCaptureItemInterop>();
    winrt::check_hresult(interop->CreateForWindow(
        handle, winrt::guid_of<ABI::Windows::Graphics::Capture::IGraphicsCaptureItem>(),
        winrt::put_abi(captureItem)));

    const int numBufferedFrames = 1;
    framePool = Direct3D11CaptureFramePool::Create(
        device.as<IDirect3DDevice>(),
        DirectXPixelFormat::R8G8B8A8UIntNormalized,
        numBufferedFrames,
        captureItem.Size());
    
    captureSession = framePool.CreateCaptureSession(captureItem);
}

WindowCapture::~WindowCapture() {
    Stop();
    framePool.Close();
}

void WindowCapture::Start() {
    captureSession.StartCapture();
}

void WindowCapture::Stop() {
    captureSession.Close();
}

FrameData WindowCapture::GetFrame() {
    auto frame = framePool.TryGetNextFrame();
    winrt::com_ptr<ID3D11Texture2D> frameTexture;

    //If next frame is not yet available, return last frame.
    if (!frame) {
        return lastFrame;
    }

    winrt::check_hresult(GetDXGIInterface(
        frame.Surface(),
        frameTexture.put()));

    if (cpuTexture == nullptr) {
        D3D11_TEXTURE2D_DESC desc = {};
        frameTexture->GetDesc(&desc);
        desc.Usage = D3D11_USAGE_STAGING;
        desc.BindFlags = 0;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        desc.MiscFlags = 0;
        d3dDevice2->CreateTexture2D(&desc, nullptr, &cpuTexture);
    }
    
    //Copy to CPU
    d3dContext->CopyResource(cpuTexture.Get(), frameTexture.get());

    D3D11_MAPPED_SUBRESOURCE mapped;
    HRESULT hr = d3dContext->Map(cpuTexture.Get(), 0, D3D11_MAP_READ, 0, &mapped);
    if (FAILED(hr)) {
        std::cerr << "Failed to map texture: " << std::hex << hr << std::endl;
        throw std::runtime_error("Failed to map texture");
    }

    D3D11_TEXTURE2D_DESC desc;
    cpuTexture->GetDesc(&desc);

    int channels = 4;
    size_t imageNumBytes = desc.Width * desc.Height * channels;
    if (lastFrame.data.size() != imageNumBytes) {
        lastFrame.data.resize(imageNumBytes);
    }
    
    size_t rowSize = desc.Width * channels;
    lastFrame.height = desc.Height;
    lastFrame.width = desc.Width;
    lastFrame.channels = channels;

    // Source starts at the top row of the DirectX image
    uint8_t* srcData = static_cast<uint8_t*>(mapped.pData);
    
    // Destination starts at the bottom row for OpenGL (flipped)
    uint8_t* dstData = lastFrame.data.data() + (desc.Height - 1) * rowSize;
    
    // Copy each row, moving source down and destination up
    for (UINT y = 0; y < desc.Height; y++) {
        std::memcpy(dstData, srcData, rowSize);
        srcData += mapped.RowPitch;   // Move to next source row (using DirectX pitch)
        dstData -= rowSize;           // Move to previous destination row (for vertical flip)
    }
    
    
    // std::memcpy(lastFrame.data.data(), mapped.pData, imageNumBytes);

    return lastFrame;
}

bool WindowCapture::WindowActive()
{
    return (GetActiveWindow() == handle);
}

Eigen::Vector2i WindowCapture::GetCursorPosition()
{
    POINT screenPos;
    if (GetCursorPos(&screenPos)) {
        POINT windowPos = screenPos;
        if (ScreenToClient(handle, &windowPos)) {
            return {windowPos.x, windowPos.y};
        }
    }
    return {-1, -1};
}
