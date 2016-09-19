#include <model.hpp>

// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;
// GL Includes
#include <GL/glew.h> // Contains all the necessery OpenGL includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <mesh.hpp>

// Constructor, expects a filepath to a 3D model.
Model::Model(const GLchar* path)
{
  this->loadModel(path);
}

// Constructor, expects a filepath to a 3D model.
Model::Model(const GLchar* path, const Texture& defaultTexture )
{
  this->defaultTexture = defaultTexture;
  this->loadModel(path);
}

// Constructor, expects a filepath to a 3D model.
Model::Model(const GLchar* path, const Texture& defaultTexture, const glm::vec2& defaultTexCoords )
{
  this->defaultTexture = defaultTexture;
  this->defaultTexCoords = defaultTexCoords;
  this->loadModel(path);
}

void Model::addInstance(glm::vec3 posDelta, glm::vec2 texDelta, glm::vec3 projDelta) {
  for(GLuint i = 0; i < this->meshes.size(); i++) {
    meshes[i].instancePositionOffset.push_back(glm::translate(glm::mat4(), posDelta ));
    meshes[i].instanceTextureOffset.push_back( texDelta);
    meshes[i].instanceProj.push_back(          glm::translate(glm::mat4(), projDelta ));
  }
}

int Model::numInstances() {
  return meshes[0].instancePositionOffset.size();
}

// Draws the model, and thus all its meshes
void Model::Draw(Shader shader)
{
  for(GLuint i = 0; i < this->meshes.size(); i++) {
    this->meshes[i].Draw(shader);
  }
}

/*  Functions   */
// Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
void Model::loadModel(string path)
{
  //fprintf(stderr, "Loading model at: %s\n", path.c_str());
  // Read file via ASSIMP
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);
  // Check for errors
  if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
  {
    cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
    return;
  }
  // Retrieve the directory path of the filepath
  this->directory = path.substr(0, path.find_last_of('/'));

  // Process ASSIMP's root node recursively
  this->processNode(scene->mRootNode, scene);
}

// Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void Model::processNode(aiNode* node, const aiScene* scene)
{
  //fprintf(stderr, "meshes: %d\n", node->mNumMeshes);
    // Process each mesh located at the current node
    for(GLuint i = 0; i < node->mNumMeshes; i++)
    {
        // The node object only contains indices to index the actual objects in the scene. 
        // The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]]; 
        this->meshes.push_back(this->processMesh(mesh, scene));     
    }
    // After we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for(GLuint i = 0; i < node->mNumChildren; i++)
    {
        this->processNode(node->mChildren[i], scene);
    }

}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    // Data to fill
    vector<Vertex> vertices;
    vector<GLuint> indices;
    vector<Texture> textures;
    //fprintf(stderr, "verticies: %d\n", mesh->mNumVertices);
    // Walk through each of the mesh's vertices
    for(GLuint i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector; // We declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // Positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;

        if (mesh->HasNormals()) {
          // Normals
          vector.x = mesh->mNormals[i].x;
          vector.y = mesh->mNormals[i].y;
          vector.z = mesh->mNormals[i].z;
          vertex.Normal = vector;
        }
        // Texture Coordinates
        if(mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x; 
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;

        }
        else {
            vertex.TexCoords = defaultTexCoords;
        }
        vertices.push_back(vertex);
    }
    // Now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for(GLuint i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // Retrieve all indices of the face and store them in the indices vector
        for(GLuint j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    textures.push_back(defaultTexture);
    
    // Return a mesh object created from the extracted mesh data
    return Mesh(vertices, indices, textures);
}




