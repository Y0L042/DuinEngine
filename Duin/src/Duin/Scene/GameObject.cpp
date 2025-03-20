#include "dnpch.h"
#include "GameObject.h"

#include "SceneManager.h"

namespace duin {
    GameObject::GameObject()
    {
    }

    GameObject::~GameObject()
    {
    }

    UUID GameObject::GetUUID()
    { 
        return uuid; 
    }

	bool GameObject::operator==(GameObject& other)
	{
		return this->GetUUID() == other.GetUUID();
	}

    SceneManager& GameObject::GetSceneManager()
    {
        return SceneManager::Get();
    }

    void GameObject::RegisterObject()
    {

    }

    void GameObject::DeregisterObject()
    {

    }
}
