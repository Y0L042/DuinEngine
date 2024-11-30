#include "dnpch.h"
#include "GameObject.h"

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
}
