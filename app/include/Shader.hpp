#ifndef SHADER_HPP
#define SHADER_HPP
#include <glad/glad.h>

#include <Eigen/Eigen>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <Windows.h>
#include <filesystem>


std::string getExecutablePath();

class Shader
{
public:
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(std::string vertexPath, std::string fragmentPath);

    // activate the shader
    // ------------------------------------------------------------------------
    void Use();

    // utility uniform functions
    void SetBool(const std::string& name, bool value) const;

    void SetInt(const std::string& name, int value) const;

    void SetFloat(const std::string& name, float value) const;

    void SetMatrix4f(const std::string& name, const Eigen::Matrix4f& matrix) const;
public:
    unsigned int ID;

private:
    // utility function for checking shader compilation/linking errors.
    void CheckCompileErrors(unsigned int shader, std::string type);
};


#endif