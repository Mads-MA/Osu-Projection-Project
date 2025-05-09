#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <FrameData.hpp>
#include <imgui.h>
#include <vector>

class Texture {
public:
    Texture();
    ~Texture();
    void SetData(const std::vector<uint8_t>& textureData, int width, int height, int channels);
    ImTextureID Get();
private:
    unsigned int textureID;
};

#endif