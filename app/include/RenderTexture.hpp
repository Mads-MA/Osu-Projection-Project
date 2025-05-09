#ifndef RENDER_TEXTURE_HPP
#define RENDER_TEXTURE_HPP

#include <Shader.hpp>
#include <array>
#include <Eigen/Eigen>

class RenderTexture {
public:
    RenderTexture();
    ~RenderTexture();
    void SetTexture(const std::vector<BYTE>& imageData, int width, int height, int channels);
    void Render();
private:
    Shader shader;

    unsigned int posVBO = 0;
    unsigned int uvVBO = 0;
    unsigned int VAO = 0;
    unsigned int textureID = 0;


    std::array<Eigen::Vector3f, 6> vertices = {
        //Topleft triangle
        Eigen::Vector3f{-1.f, 1.f, 0.f},
        Eigen::Vector3f{1.f, 1.f, 0.f},
        Eigen::Vector3f{-1.f, -1.f, 0.f},

        //Bottom right triangle
        Eigen::Vector3f{-1.f, -1.f, 0.f},
        Eigen::Vector3f{1.f, 1.f, 0.f},
        Eigen::Vector3f{1.f, -1.f, 0.f},
    };

    std::array<Eigen::Vector2f, 6> uvCoords = {
        //Topleft triangle
        Eigen::Vector2f{0.f, 1.f},
        Eigen::Vector2f{1.f, 1.f},
        Eigen::Vector2f{0.f, 0.f},

        //Bottom right triangle
        Eigen::Vector2f{0.f, 0.f},
        Eigen::Vector2f{1.f, 1.f},
        Eigen::Vector2f{1.f, 0.f},
    };


};

#endif