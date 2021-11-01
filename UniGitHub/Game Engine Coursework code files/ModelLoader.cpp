#include "ModelLoader.h"
#define STB_IMAGE_IMPLEMENTATION
#include <\FromMSI\Uni\301CR\301EngineCoursework\CarParkPuzzle\assimp-5.0.1\contrib\stb_image\stb_image.h>

// This is purely for loading textures from a given file path.
unsigned int TextureFromFile(const char* path, const std::string& directory);

/*
This constructs an object that loads the model. 
*/
ModelLoader::ModelLoader(std::string const& path, Shader& shader){
	stbi_set_flip_vertically_on_load(true);
	LoadModel(path, shader);
}

/*
Basic deconstructor.
*/
ModelLoader::~ModelLoader() {

}

/*
This loops through the meshes and calls thr draw function in the mesh loader. I have to put this here
	as the program cannot directly access the mesh loaders draw function due to the object being of the 
	ModelLoader class, not the MeshLoader class.
*/
void ModelLoader::Draw(Shader& shader) {
	for (unsigned int i = 0; i < meshes.size(); i++) {
		SetMaterialProperties(materials[i], shader);
		meshes[i].Draw(shader);
	}
}

/*
This begins the Assimp functionality by creating an importer and a scene, setting the directory,
	and then processing nodes beginning with the root node.
*/
void ModelLoader::LoadModel(std::string const& path, Shader& shader) {
	Assimp::Importer importer;
	scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "Error with Assimp: " << importer.GetErrorString() << std::endl;
		return;
	}

	directory = path.substr(0, path.find_last_of('/'));

	ProcessNode(scene->mRootNode, scene, shader);
}

/*
This is a recursive procedure that processes a node and then all of that nodes children.
	To completely process the node, the meshes for that node needs to be processed, so 
	another function is called that deals with that and the mesh is added to a vector.
*/
void ModelLoader::ProcessNode(aiNode* node, const aiScene* scene, Shader& shader) {
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(ProcessMesh(mesh, scene, shader));
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		ProcessNode(node->mChildren[i], scene, shader);
	}
}

/*
This is where most of the work for model loading takes place. Each vertex is looped through, 
	normals are found and added if they exist, texture coordinates are too if they exist, 
	and materials are also loaded here. Currently I have commented out the texture loading,
	as my models only have material applied.
*/
MeshLoader ModelLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene, Shader& shader) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		glm::vec3 vektor; // Named this way to reduce confusion between this and std::vector
		vektor.x = mesh->mVertices[i].x;
		vektor.y = mesh->mVertices[i].y;
		vektor.z = mesh->mVertices[i].z;
		vertex.position = vektor;

		if (mesh->HasNormals()) {
			vektor.x = mesh->mNormals[i].x;
			vektor.y = mesh->mNormals[i].y;
			vektor.z = mesh->mNormals[i].z;
			vertex.normal = vektor;
		}

		if (mesh->mTextureCoords[0]) {
			glm::vec2 vec;

			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.textureCoordinates = vec;

			vektor.x = mesh->mTangents[i].x;
			vektor.y = mesh->mTangents[i].y;
			vektor.z = mesh->mTangents[i].z;
			vertex.tangent = vektor;

			vektor.x = mesh->mBitangents[i].x;
			vektor.y = mesh->mBitangents[i].y;
			vektor.z = mesh->mBitangents[i].z;
			vertex.bitangent = vektor;
		}
		else {
			vertex.textureCoordinates = glm::vec2(0.0f, 0.0f);
		}

		vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	if (mesh->mMaterialIndex >= 0) {
		matLoad = scene->mMaterials[mesh->mMaterialIndex];
		mater = loadMaterial(matLoad);
		materials.push_back(mater);

		// This is for if a model I was going to load had textures applied, not just materials.
		/*std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());*/
	}

	return MeshLoader(vertices, indices, textures);
}

/*
This is where the texture is loaded. This is optimised in a way where it will not load the same texture
	twice, which I have accomplished using a Boolean that says whether or not to skip adding the texture 
	to the vector.
*/
std::vector<Texture> ModelLoader::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++) {
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip) {
			Texture texture;
			texture.ID = TextureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}
	}
	return textures;
}

/*
This loads the texture image given a file path. This uses the stb_image.h file which has its own include
	and definition. This had to be included in the .cpp for this class as if I include it in the header,
	when I go to include the class elsewhere, an error arises where the header is apprently being loaded
	twice. Adding '#pragma once' to stb_image.h did not seem to fix the issue, so I included it here 
	instead.
*/
unsigned int TextureFromFile(const char* path, const std::string& directory) {
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format;
		if (nrComponents == 1) {
			format = GL_RED;
		}
		else if (nrComponents == 3) {
			format = GL_RGB;
		}
		else if (nrComponents == 4) {
			format = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		std::cout << "Texture failed to load at path " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

/*
This loads and creates a material type that includes the 4 different main aspects of a material.
*/
Material ModelLoader::loadMaterial(aiMaterial* mat) {
	Material material;
	aiColor3D diffuse(0.0f, 0.0f, 0.0f);
	aiColor3D ambient(0.0f, 0.0f, 0.0f);
	aiColor3D specular(0.0f, 0.0f, 0.0f);
	float shininess;

	mat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
	material.diffuse = glm::vec3(diffuse.r, diffuse.g, diffuse.b);

	mat->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
	material.ambient = glm::vec3(ambient.r, ambient.g, ambient.b);

	mat->Get(AI_MATKEY_COLOR_SPECULAR, specular);
	material.specular = glm::vec3(specular.r, specular.g, specular.b);

	mat->Get(AI_MATKEY_SHININESS, shininess);
	material.shininess = shininess;

	return material;
}

/*
This sets the material in the fragment shader and is called in every render loop.
*/
void ModelLoader::SetMaterialProperties(Material mat, Shader& shader) {
	glUniform3f(glGetUniformLocation(shader.ID, "material.ambient"), mat.ambient.x, mat.ambient.y, mat.ambient.z);
	glUniform3f(glGetUniformLocation(shader.ID, "material.diffuse"), mat.diffuse.x, mat.diffuse.y, mat.diffuse.z);
	glUniform3f(glGetUniformLocation(shader.ID, "material.specular"), mat.specular.x, mat.specular.y, mat.specular.z);
	//glUniform3f(glGetUniformLocation(shader.ID, "material.specular"), 0, 0, 0);
	shader.SetFloat("material.shiniess", mat.shininess);
}