#include "dnpch.h"

#include "Object.h"

namespace Duin
{
	Object::Object()
	{
	}

	Object::~Object()
	{
	}
	bool Object::operator==(Object& other)
	{
		return this->GetUUID() == other.GetUUID();
	}
}