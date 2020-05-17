//
//  mesh.h
//  RefractionProject
//
//  Created by Daniel on 2020-05-07.
//  Copyright © 2020 Litinc. All rights reserved.
//

#ifndef mesh_h
#define mesh_h

#include <glad/glad.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "shader.hpp"

#include <string>
#include <vector>
using namespace std;

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture {
    unsigned int id;
    string type; //e.g diffuse or specular map
    string path;
};

class Mesh {
public:
    //mesh data
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    unsigned int VAO;
    
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures) {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        //glfwInit();
        setupMesh();
    }
    void Draw(Shader shader, bool renderBack) {
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        for(unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
            // retrieve texture number (the N in diffuse_textureN)
            string number;
            string name = textures[i].type;
            if(name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if(name == "texture_specular")
                number = std::to_string(specularNr++);

            shader.setFloat(("material." + name + number).c_str(), i);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }
        glActiveTexture(GL_TEXTURE0);
        //Activate shader???
        // draw mesh
        glBindVertexArray(VAO);
        /*
        if(renderBack) {
            glClearDepth(0.0f);
            glDepthFunc(GL_GREATER);
        } else {
            glDepthFunc(GL_LESS);
        }*/
        //glClearDepth(0.0f);
        //glDepthFunc(GL_GREATER);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0); //Remove the VAO settings
    }
private:
    //render data
    /*
     VBO - where the vertex shader input is linked to.
     Bound VBO to GL_ARRAY_BUFFER and every operation on vertex attributes will
     take from the VBO that is connected to GL_ARRAY_BUFFER, Ex when transfering a vertex attr to GPU.
     
     VAO - where the mesh settings are saved, set it with
     glBindVertexArray().
     
     EBO - where the indices for primitives are saved, can be used to
     prevent vertices from being drawn more than one time.
     */
    unsigned int VBO, EBO;
    void setupMesh() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0] ,GL_STATIC_DRAW);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        
        //vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        //vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        //vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    }
};

#endif /* mesh_h */
