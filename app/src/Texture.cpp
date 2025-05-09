#include <Texture.hpp>
#include <glad/glad.h>


Texture::Texture() {
    glGenTextures(1, &textureID);
}

Texture::~Texture() {
    glDeleteTextures(1, &textureID);
}

void Texture::SetData(const std::vector<uint8_t> &textureData, int width, int height, int channels) {
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    GLenum format = GL_RGBA;
    if (channels == 3) {
        format = GL_RGB;
    }
    else if (channels == 1) {
        format = GL_RED;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 
        0, format, GL_UNSIGNED_BYTE, textureData.data());

    glBindTexture(GL_TEXTURE_2D, 0);
}

ImTextureID Texture::Get() {
    return (ImTextureID)textureID;
}
