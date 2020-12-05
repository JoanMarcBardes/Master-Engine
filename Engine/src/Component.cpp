#include "Component.h"

Component::Component(Type type, GameObject* gameObject) : type(type), gameObject(gameObject)
{
}

Component::~Component()
{

}