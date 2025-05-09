#include <RenderTexture.hpp>

#include <glad/glad.h>

RenderTexture::RenderTexture() : shader("/assets/shaders/shader.vert", "/assets/shaders/shader.frag") {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &posVBO);
    glGenBuffers(1, &uvVBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, posVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uvCoords), uvCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

RenderTexture::~RenderTexture() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &posVBO);
    glDeleteBuffers(1, &uvVBO);
}

void RenderTexture::SetTexture(const std::vector<BYTE>& imageData, int width, int height, int channels) {
    if (textureID == 0) {
        glGenTextures(1, &textureID);
        
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else {
        glBindTexture(GL_TEXTURE_2D, textureID);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,  0, GL_RGBA, GL_UNSIGNED_BYTE, imageData.data());

    glGenerateMipmap(GL_TEXTURE_2D);

}   

void RenderTexture::Render() {
    shader.Use();
    glBindTexture(GL_TEXTURE_2D, textureID);    
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
