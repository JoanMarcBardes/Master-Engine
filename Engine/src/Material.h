#pragma once
#include "Component.h"
#include <vector>
#include <string>

class Material : public Component
{
public:
	Material();
	Material(std::vector<unsigned int> texturesList, std::vector<std::string> pathList, std::vector<unsigned int> typeIdList);

	void AddTexturePath(unsigned int texture, std::string path, unsigned int typeId);

	void Draw(const unsigned program);

	const std::vector<unsigned int> GetTextures() const { return textures; }
	const std::vector<std::string> GetPaths() const { return paths; }
	const std::vector<unsigned int> GetTypeId() const { return typeId; }
	const unsigned int GetNunTextures() const { return textures.size(); }
	static Type GetType() { return Type::Material; };

private:
	std::vector<unsigned int> textures;
	std::vector<std::string> paths;
	std::vector<unsigned int> typeId; // 0 = Diffuse, 1 = Specular
};

