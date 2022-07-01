#include "Model.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace {
    unsigned int textureFromFile(const char* path, const std::string& directory, bool gamma = false) {
        std::string filename = std::string(path);
        filename = directory + '/' + filename;

        unsigned int textureId;
        glGenTextures(1, &textureId);

        int width, height, nrComponents;
        unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureId);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            // set the texture wrapping parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            // set texture filtering parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
            std::cerr << "ERROR::TEXTURE::CANNOT LOAD TEXTURE" << std::endl;
            stbi_image_free(data);
        }

        return textureId;
    }
}

Model::Model(std::string const& path, bool gamma) : gammaCorrection(gamma) {
	loadModel(path);
}

// -------------------------------------------------------------------------------------------------------------------------------------
void Model::loadModel(std::string path) {
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene);
}

// -------------------------------------------------------------------------------------------------------------------------------------
void Model::draw (Shader& shader) {
    for (unsigned int i = 0; i < meshes.size(); i++) {
        meshes[i].draw(shader);
    }
}

// -------------------------------------------------------------------------------------------------------------------------------------
std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++) {
            if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
                textures.push_back(textures_loaded[j]);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        if (!skip) {   // if texture hasn't been loaded already, load it
            Texture texture;
            texture.id = textureFromFile(str.C_Str(), this->directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
        }
    }
    return textures;
}

// -------------------------------------------------------------------------------------------------------------------------------------
void Model::processNode(aiNode* node, const aiScene* scene) {
    // process all node's meshes if any
    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    // recurse for children node
    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        processNode(node->mChildren[i], scene);
    }
}

// -------------------------------------------------------------------------------------------------------------------------------------
Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex>  vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        Vertex vertex;
        glm::vec3 posVec;
        
        posVec.x = mesh->mVertices[i].x;
        posVec.y = mesh->mVertices[i].y;
        posVec.z = mesh->mVertices[i].z;
        vertex.position = posVec;

        glm::vec3 normVec;
        normVec.x = mesh->mNormals[i].x;
        normVec.y = mesh->mNormals[i].y;
        normVec.z = mesh->mNormals[i].z;
        vertex.normal = normVec;

        if (mesh->mTextureCoords[0]) {
            glm::vec2 texCoord;
            texCoord.x = mesh->mTextureCoords[0][i].x;
            texCoord.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoord = texCoord;

            // tangent
            glm::vec3 tanVec;
            tanVec.x = mesh->mTangents[i].x;
            tanVec.y = mesh->mTangents[i].y;
            tanVec.z = mesh->mTangents[i].z;
            vertex.tangent = tanVec;

            // bitangent
            glm::vec3 bitanVec;
            bitanVec.x = mesh->mBitangents[i].x;
            bitanVec.y = mesh->mBitangents[i].y;
            bitanVec.z = mesh->mBitangents[i].z;
            vertex.bitangent = bitanVec;
        }
        else vertex.texCoord = glm::vec2(0.f);

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        aiFace face = mesh->mFaces[i];

        for (unsigned int j = 0; j < face.mNumIndices; ++j) {
            indices.push_back(face.mIndices[j]);
        }
    }

    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
    }

    return Mesh(vertices, indices, textures);
}
