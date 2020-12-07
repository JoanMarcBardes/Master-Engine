#include "Material.h"
#include "GL/glew.h"

Material::Material(std::vector<unsigned int> textures, std::vector<std::string> paths) :
	Component(Component::Type::Material, gameObject), textures(textures), paths(paths)
{

}

void Material::AddTexturePath(unsigned int texture, std::string path)
{
	textures.push_back(texture);
	paths.push_back(path);
}

void Material::Draw(const unsigned program)
{
    glActiveTexture(GL_TEXTURE0);
    //for (unsigned int i = 0; i < texturesList.size(); ++i) {
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glUniform1i(glGetUniformLocation(program, "my_texture"), 0);
    //}
}
