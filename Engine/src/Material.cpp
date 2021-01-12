#include "Material.h"
#include "GL/glew.h"
#include "Globals.h"

Material::Material() : Component(Component::Type::Material, gameObject), textures(), paths(), typeId()
{
}

Material::Material(std::vector<unsigned int> textures, std::vector<std::string> paths, std::vector<unsigned int> typeIdList) :
	Component(Component::Type::Material, gameObject), textures(textures), paths(paths), typeId(typeIdList)
{

}

void Material::AddTexturePath(unsigned int texture, std::string path, unsigned int newtypeId)
{
	textures.push_back(texture);
	paths.push_back(path);
    typeId.push_back(newtypeId);
}

void Material::Draw(const unsigned program)
{
    std::string name;
    unsigned int has_diffuse = 0;
    unsigned int has_specular = 0;
    unsigned int shininess_alpha = 0;
    char* nameType = "";

    for (unsigned int i = 0; i < textures.size(); ++i) {
        if (typeId[i] == 0)
        {
            nameType = "diffuse_map";
            has_diffuse = 1;
        }
        else if (typeId[i] == 1)
        {
            nameType = "specular_map";
            has_specular = 1;
            shininess_alpha = 1;
        }
        else LOG("[error] material draw unknown type");

        glActiveTexture(GL_TEXTURE0 + i);
        glUniform1i(glGetUniformLocation(program, nameType), i);
        glBindTexture(GL_TEXTURE_2D, textures[i]);
    }
    //glUniform1i(glGetUniformLocation(program, "has_diffuse_map"), has_diffuse);
    //glUniform1i(glGetUniformLocation(program, "has_specular_map"), has_specular);
    //glUniform1i(glGetUniformLocation(program, "shininess_alpha"), shininess_alpha);


    glActiveTexture(GL_TEXTURE0);
}
