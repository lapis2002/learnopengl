#pragma once
#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Shader.h>
#include <Mesh.h>

#include <vector>
#include <string>

class Model {
private:
	std::vector<Mesh> meshes;
	std::string directory;

	std::vector<Texture> textures_loaded;

	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
public:
	bool gammaCorrection;

	Model(std::string const& path, bool gamma = false);
	void draw(Shader& shader);
};

#endif

