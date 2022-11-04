#pragma once

#include <unordered_map>
#include <string>
#include <memory>

#include <GLAD/glad.h>

#include <engine/ecs/Scene.h>
#include <engine/renderer/Buffers.h>
#include <engine/material/Material.h>
#include <engine/renderer/Texture.h>
#include <engine/model/Model.h>

// Static singleton to keep all resources needed by meshes
// or direct draw calls

class ResManager
{
public:

	static std::unordered_map<std::string, Amba::Scene*>				rm_Scenes;
	static std::unordered_map<std::string, Amba::Shader*>				rm_Shaders;
	static std::unordered_map<std::string, Amba::Material*>				rm_Materials;
	static std::unordered_map<std::string, Amba::Texture*>				rm_Textures;
	static std::unordered_map<std::string, Amba::Model*>				rm_Models;

	static void CreateScene(const std::string& name);
	static void CreateShader(const char* vsPath, const char* fsPath, const std::string& name);
	static void CreateMaterial(const std::string& name);
	static void GenTexture(const std::string& filePat, const std::string& name);
	static void CreateModel(const std::string& name);

	static Amba::Scene* GetScene(const std::string& name);
	static Amba::Shader* GetShader(const std::string& name);
	static Amba::Texture* GetTexture(const std::string& name);
	static Amba::Model* GetModel(const std::string& name);
	static Amba::Material* GetMaterial(const std::string& name);


	static void cleanup();

private:

};
