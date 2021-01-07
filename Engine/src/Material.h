#pragma once
#include "Component.h"
#include <vector>
#include <string>

class Material : public Component
{
public:
	Material(std::vector<unsigned int> texturesList, std::vector<std::string> pathList, std::vector<std::string> typeIdList);

	void AddTexturePath(unsigned int texture, std::string path, std::string typeId);

	void Draw(const unsigned program);

	const std::vector<unsigned int> GetTextures() { return textures; }
	const std::vector<std::string> GetPaths() { return paths; }
	const std::vector<std::string> GetTypeId() { return typeId; }
	const unsigned int GetNunTextures() const { return textures.size(); }
	static Type GetType() { return Type::Material; };

private:
	std::vector<unsigned int> textures;
	std::vector<std::string> paths;
	std::vector<std::string> typeId;
};

