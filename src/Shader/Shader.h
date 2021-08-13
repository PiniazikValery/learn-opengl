#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h> // подключаем glad для активации всех заголовочных файлов OpenGL
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

class Shader
{
public:
    // ID - идентификатор программы
    unsigned int ID;

    // Конструктор считывает данные и выполняет построение шейдера
    Shader(const char *vertexPath, const char *fragmentPath);

    // Использование/активация шейдера
    void use();

    // Полезные uniform-функции
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void set4f(const std::string &name, const std::vector<float> &vec4f) const;
    void setMat3(const std::string &name, const glm::mat3 &mat) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;
    void setVec3(const std::string &name, float x, float y, float z) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;

private:
    void checkCompileErrors(unsigned int shader, std::string type);
};

#endif