#ifndef MESH_H
#define MESH_H
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <iostream>
#include <vector>
#include <string>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture
{
    unsigned int id;
    const char* type; 
    const char* path; 
};

class Mesh 
{
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    Mesh(std::vector<Vertex> verticies, std::vector<unsigned int> indices, std::vector<Texture> textures)
    {
        this->vertices = verticies;
        this->indices = indices;
        this->textures = textures;
        setupMesh();
    }


    void Draw()
    {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    };
private:
    void setupMesh()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
        glEnableVertexAttribArray(0);
        
        // Normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        glEnableVertexAttribArray(1);
        
        // Texture coordinate attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }
    unsigned int VAO, VBO, EBO;
};

class Model
{
public:
    Model(std::string const path = "", bool isCube = false)
    {
        if(!isCube)
        {
            loadModel(std::string(ASSET_DIR) + path);
        }
        /*
        Following section was gererated from ChatGPT found at https://chatgpt.com/
        Generated using the prompt: 

        "I've got a script to create a model from a file using assimp but i'd like to have a default cube as an option under the "model" class, 
        can you add the verticies uvs an normals as float co-ordinate data in the else part of  the !isCube if statement, or just as a separate vairable, 
        i just need separate arrays of the individual position UV and normal data"

        [Relevant functions listed in prompt]

        CITATION BEGIN:
        */
        else
        {
            std::vector<Vertex> cubeVerts = {
            // Front face
            {{-1.0f, -1.0f,  1.0f}, { 0.0f,  0.0f, 1.0f}, {0.0f, 0.0f}},
            {{ 1.0f, -1.0f,  1.0f}, { 0.0f,  0.0f, 1.0f}, {1.0f, 0.0f}},
            {{ 1.0f,  1.0f,  1.0f}, { 0.0f,  0.0f, 1.0f}, {1.0f, 1.0f}},
            {{-1.0f,  1.0f,  1.0f}, { 0.0f,  0.0f, 1.0f}, {0.0f, 1.0f}},

            // Back face
            {{ 1.0f, -1.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}},
            {{-1.0f, -1.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}},
            {{-1.0f,  1.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}},
            {{ 1.0f,  1.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}},

            // Left face
            {{-1.0f, -1.0f, -1.0f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
            {{-1.0f, -1.0f,  1.0f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
            {{-1.0f,  1.0f,  1.0f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
            {{-1.0f,  1.0f, -1.0f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},

            // Right face
            {{ 1.0f, -1.0f,  1.0f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
            {{ 1.0f, -1.0f, -1.0f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
            {{ 1.0f,  1.0f, -1.0f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
            {{ 1.0f,  1.0f,  1.0f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},

            // Top face
            {{-1.0f,  1.0f,  1.0f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}},
            {{ 1.0f,  1.0f,  1.0f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}},
            {{ 1.0f,  1.0f, -1.0f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}},
            {{-1.0f,  1.0f, -1.0f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}},

            // Bottom face
            {{-1.0f, -1.0f, -1.0f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}},
            {{ 1.0f, -1.0f, -1.0f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}},
            {{ 1.0f, -1.0f,  1.0f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}},
            {{-1.0f, -1.0f,  1.0f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}},
            };

            std::vector<unsigned int> cubeIndices = 
            {
                // Front face
                0, 1, 2,  2, 3, 0,
                // Back face
                4, 5, 6,  6, 7, 4,
                // Left face
                8, 9,10, 10,11, 8,
                // Right face
                12,13,14, 14,15,12,
                // Top face
                16,17,18, 18,19,16,
                // Bottom face
                20,21,22, 22,23,20
            };

            meshes.push_back(Mesh(cubeVerts, cubeIndices, {}));
        }
        // END OF CITATION
    }

    void Draw()
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw();
    }

    void translate(glm::vec3 trans)
    {
        transMat = glm::translate(transMat, trans);
        pos += trans;
    }
    void scale (glm::vec3 s)
    {
        transMat = glm::scale(transMat,s);
        sca = s;
    }
    std::vector<Mesh> meshes;
    std::string directory;

    glm::vec3 pos = glm::vec3(0.0,0.0,0.0);
    glm::vec3 rot;
    glm::vec3 sca;

    glm::mat4 transMat = glm::mat4(1.0f);
private:


    void loadModel(const std::string &path)
    {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return;
        }

        if (scene->mFlags & AI_SCENE_FLAGS_NON_VERBOSE_FORMAT)
        {
            std::cout << "Non-verbose format detected, some features may not work as expected." << std::endl;
        }
        directory = path.substr(0, path.find_last_of('/'));

        processNode(scene->mRootNode, scene);
        transMat = glm::translate(transMat,pos);

    }
    void processNode(aiNode *node, const aiScene *scene)
    {
        for(unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        for(unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }

    }
    Mesh processMesh(aiMesh *mesh, const aiScene *scene)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;

            glm::vec3 vector;
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;

            if (mesh->HasNormals()) 
            {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
            }

            if (mesh->mTextureCoords[0]) 
            {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            }
            else
            {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            }
            vertices.push_back(vertex);

        }
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }
        return Mesh(vertices, indices, loadMaterialTextures(scene->mMaterials[mesh->mMaterialIndex], aiTextureType_DIFFUSE, "texture_diffuse"));

    }
    
    std::vector<Texture> loadMaterialTextures(aiMaterial *material, aiTextureType type, const std::string& typeName)
    {
        return {};
    }
};


#endif