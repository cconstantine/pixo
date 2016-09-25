#include <led_cluster.hpp>
#include <math.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

LedCluster::LedCluster(const Texture& texture)
: defaultTexture(texture), leds(GL_POINTS),
  plane("../models/plane.obj", texture),
  ds(7331), buffer_size(0), gamma(0.5)
{
  setGamma(gamma);
  // Assimp::Importer importer;

  // model = importer.ReadFile("../models/dome.obj", aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);
  // processNode(model->mRootNode, model);
  this->loadModel("../models/dome.obj");

  for(int i = -5;i < 5;i++) {
    for(int j = -50;j < 50;j++) {
      glm::vec3 offset(i*7.0f, 0*7.0f, j*7.0f);

      std::string mac3("d8:80:39:66:4b:de");
      addStrip(mac3, 0,  0,  28,  2, 84, offset);
      addStrip(mac3, 0, 84,   2, 22, 82, offset);
      addStrip(mac3, 1,  0,  28, 22, 84, offset);
      addStrip(mac3, 1, 84,  22,  0, 72, offset);
      addStrip(mac3, 2,  0,  28, 24, 84, offset);
      addStrip(mac3, 2, 84,  24, 22, 82, offset);
      addStrip(mac3, 3,  0,  28, 29, 84, offset);
      addStrip(mac3, 3, 84,  29, 24, 82, offset);
      addStrip(mac3, 4,  0,  28, 30, 84, offset);
      addStrip(mac3, 4, 84,  30, 29, 82, offset);
      addStrip(mac3, 5,  0,  28,  4, 84, offset);
      addStrip(mac3, 5, 84,   4, 30, 82, offset);
      addStrip(mac3, 6,  0,  30, 44, 84, offset);
      addStrip(mac3, 6, 84,  44, 29, 84, offset);

      std::string mac2("d8:80:39:66:48:44");
      addStrip(mac2, 0,  0,  23, 22, 84, offset);
      addStrip(mac2, 0, 84,  22, 16, 82, offset);
      addStrip(mac2, 1,  0,  23, 16, 84, offset);
      addStrip(mac2, 1, 84,  16,  0, 72, offset);
      addStrip(mac2, 2,  0,  23, 18, 84, offset);
      addStrip(mac2, 2, 84,  18, 16, 82, offset);
      addStrip(mac2, 3,  0,  23, 25, 84, offset);
      addStrip(mac2, 3, 84,  25, 18, 82, offset);
      addStrip(mac2, 4,  0,  23, 26, 84, offset);
      addStrip(mac2, 4, 84,  26, 25, 82, offset);
      addStrip(mac2, 5,  0,  23, 24, 84, offset);
      addStrip(mac2, 5, 84,  24, 26, 82, offset);
      addStrip(mac2, 6,  0,  26, 41, 84, offset);
      addStrip(mac2, 6, 84,  41, 25, 84, offset);

      std::string mac1("d8:80:39:65:f1:91");
      addStrip(mac1, 0,  0,  17, 16, 84, offset);
      addStrip(mac1, 0, 84,  16, 10, 82, offset);
      addStrip(mac1, 1,  0,  17, 10, 84, offset);
      addStrip(mac1, 1, 84,  10,  0, 72, offset);
      addStrip(mac1, 2,  0,  17, 12, 84, offset);
      addStrip(mac1, 2, 84,  12, 10, 82, offset);
      addStrip(mac1, 3,  0,  17, 19, 84, offset);
      addStrip(mac1, 3, 84,  19, 12, 82, offset);
      addStrip(mac1, 4,  0,  17, 20, 84, offset);
      addStrip(mac1, 4, 84,  20, 19, 82, offset);
      addStrip(mac1, 5,  0,  17, 18, 84, offset);
      addStrip(mac1, 5, 84,  18, 20, 82, offset);
      addStrip(mac1, 6,  0,  20, 38, 84, offset);
      addStrip(mac1, 6, 84,  38, 19, 84, offset);
      
      std::string mac5("d8:80:39:66:0d:7e");
      addStrip(mac5, 0,  0,  11, 10, 84, offset);
      addStrip(mac5, 0, 84,  10,  1, 82, offset);
      addStrip(mac5, 1,  0,  11,  1, 84, offset);
      addStrip(mac5, 1, 84,   1,  0, 72, offset);
      addStrip(mac5, 2,  0,  11,  5, 84, offset);
      addStrip(mac5, 2, 84,   5,  1, 82, offset);
      addStrip(mac5, 3,  0,  11, 13, 84, offset);
      addStrip(mac5, 3, 84,  13,  5, 82, offset);
      addStrip(mac5, 4,  0,  11, 14, 84, offset);
      addStrip(mac5, 4, 84,  14, 13, 82, offset);
      addStrip(mac5, 5,  0,  11, 12, 84, offset);
      addStrip(mac5, 5, 84,  12, 14, 82, offset);
      addStrip(mac5, 6,  0,  14, 35, 84, offset);
      addStrip(mac5, 6, 84,  35, 13, 84, offset);

      std::string mac4("d8:80:39:66:29:c6");
      addStrip(mac4, 0,  0,   3,  1, 84, offset);
      addStrip(mac4, 0, 84,   1,  2, 82, offset);
      addStrip(mac4, 1,  0,   3,  2, 84, offset);
      addStrip(mac4, 1, 84,   2,  0, 72, offset);
      addStrip(mac4, 2,  0,   3,  4, 84, offset);
      addStrip(mac4, 2, 84,   4,  2, 82, offset);
      addStrip(mac4, 3,  0,   3,  6, 84, offset);
      addStrip(mac4, 3, 84,   6,  4, 82, offset);
      addStrip(mac4, 4,  0,   3,  8, 84, offset);
      addStrip(mac4, 4, 84,   8,  6, 82, offset);
      addStrip(mac4, 5,  0,   3,  5, 84, offset);
      addStrip(mac4, 5, 84,   5,  8, 82, offset);
      addStrip(mac4, 6,  0,   8, 32, 84, offset);
      addStrip(mac4, 6, 84,  32,  6, 84, offset);
    }
  }
  leds.setupMesh();

  // std::vector<Texture> textures;
  // textures.push_back(texture);

  // std::vector<GLuint> indices;
  // for(int i = 0;i < vertexes.size();i++) {
  //   indices.push_back(i);
  // }
  // meshes.push_back( Mesh(vertexes, indices, textures, GL_POINTS) );

 fprintf(stderr, "LEDS: %d\n", numLeds());
}



// Draws the model, and thus all its meshes
void LedCluster::Draw(Shader shader)
{
  this->leds.Draw(shader); 
  // for(GLuint i = 0; i < this->meshes.size(); i++) {
  //   this->meshes[i].Draw(shader);
  // }
}

/*  Functions   */
// Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
void LedCluster::loadModel(string path)
{
  // fprintf(stderr, "LedCluster::loadModel -> Loading model at: %s\n", path.c_str());
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

  // for(int i = 0; i < meshes.size();i++) {
  //     meshes[i].setupMesh();
  // }
}

// Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void LedCluster::processNode(aiNode* node, const aiScene* scene)
{
  // fprintf(stderr, "LedCluster::processMesh: meshes: %d\n", node->mNumMeshes);
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

Mesh LedCluster::processMesh(aiMesh* mesh, const aiScene* scene)
{
  // fprintf(stderr, "LedCluster::processMesh\n");
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
        indices.push_back(i);
    }

    textures.push_back(defaultTexture);
    
    // Return a mesh object created from the extracted mesh data
    return Mesh(vertices, indices, textures, GL_POINTS);
}






void LedCluster::setGamma(float g) {
  gamma = g;

  for(int i = 0;i < 256;i++) {
    lut[i] = pow(i, gamma);
  }
}

GLuint LedCluster::numLeds() {
  return buffer_size / 3;
}

void LedCluster::update(std::vector<uint8_t> &frameBuffer) {

  for(auto i: strip_mappings) {
    std::string mac_address = i.first;
    std::vector<Strip> strips = i.second;

    if(ds.pushers.find(mac_address) != ds.pushers.end()) {
      std::shared_ptr<PixelPusher> pusher = ds.pushers[mac_address];

      for(auto strip: strips) {
        pusher->update(strip.strip, &frameBuffer[strip.offset], strip.size, strip.strip_offset);
      }
      pusher->send();
    }

  }
}



void LedCluster::addStrip(std::string &mac, int strip, int strip_offset, int start, int end, int divisions, glm::vec3 offset) {
  int byte_offset = buffer_size;
  strip_mappings[mac].push_back(Strip(strip, strip_offset*3, byte_offset, divisions*3));
  buffer_size += divisions*3;

  addStrip(start, end, divisions, offset);
}

void LedCluster::addStrip(int start, int end, int divisions, glm::vec3 offset) {

  glm::vec3 vertex_start = meshes[0].vertices[start].Position + offset;
  glm::vec3 vertex_end =   meshes[0].vertices[end].Position + offset;
  glm::vec3 vertex_delta = vertex_end - vertex_start ;


  glm::vec2 texture_start = meshes[0].vertices[start].TexCoords;
  glm::vec2 texture_end   = meshes[0].vertices[end].TexCoords;

  glm::vec2 texture_delta = texture_end - texture_start;


  for(int i = 0;i < divisions;i++) {
    glm::vec3 ballPosDelta = vertex_start  + vertex_delta  * (1.0f/divisions)*float(i);
    glm::vec2 texDelta     = texture_start + texture_delta * (1.0f/divisions)*float(i);
    
    int count = 1;//plane.numInstances();
    int x = count % 1000;
    int y = count / 1000;
    glm::vec3 planePosDelta((float)x, (float)y, 0.0f);

    Vertex vertex;
    vertex.Position = ballPosDelta; 
    vertex.TexCoords = texDelta;
    vertex.proj = ballPosDelta;

    leds.vertices.push_back(vertex);
    leds.indices.push_back(leds.indices.size());
    //balls.addInstance(ballPosDelta, texDelta, ballPosDelta);
    //plane.addInstance(planePosDelta, texDelta, ballPosDelta);
  }
}


LedCluster::Strip::Strip() :strip(0), strip_offset(0), offset(0), size(0) { }

LedCluster::Strip::Strip(int strip, int strip_offset, int offset, int size) :
  strip(strip), strip_offset(strip_offset), offset(offset), size(size) { }

LedCluster::Strip::Strip(const Strip& copy) :
  strip(copy.strip), strip_offset(copy.strip_offset), offset(copy.offset), size(copy.size) { }