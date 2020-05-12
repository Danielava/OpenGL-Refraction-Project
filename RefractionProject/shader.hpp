//
//  shader.hpp
//  RefractionProject
//
//  Code provided by learnopengl.com
//  https://learnopengl.com/Getting-started/Shaders
//

#ifndef shader_hpp
#define shader_hpp

#include <glad/glad.h>

#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
    unsigned int ID;
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
    void use();
    void setBool();
    void setInt();
    void setFloat(const std::string &name, float value) const;
private:
    void checkCompileErrors(unsigned int shader, std::string type);
};

#endif /* shader_hpp */
