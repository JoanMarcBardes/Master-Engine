#pragma once
#include "Component.h"
#include <vector>
#include <string>

class Material : public Component
{
public:
	Material(std::vector<unsigned int> texturesList, std::vector<std::string> pathList, std::vector<unsigned int> typeIdList);

	void AddTexturePath(unsigned int texture, std::string path, unsigned int typeId);
	unsigned int SetTexture(unsigned int texture, std::string path, unsigned int typeId);
	bool GetTexture(unsigned int typeId, unsigned int &texture, std::string &path );

	void Draw(const unsigned program);

	std::vector<unsigned int> GetTextures() { return textures; }
	std::vector<std::string> GetPaths() { return paths; }
	std::vector<unsigned int> GetTypesId() { return typeId; }
	static Type GetType() { return Type::Material; };

private:
	std::vector<unsigned int> textures;
	std::vector<std::string> paths;
	std::vector<unsigned int> typeId; // 0 = Diffuse, 1 = Specular
};

