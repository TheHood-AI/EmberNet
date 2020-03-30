#include "pch.h"
#include "GameObject.h"

void GameObject::SetID(unsigned aID)
{
	myID = aID;
}

unsigned GameObject::GetID() const
{
	return myID;
}

const CU::Vector2<float>& GameObject::GetPos() const
{
	return myPos;
}

void GameObject::SetPosition(const VECTOR2F& aPos)
{
	myPos = aPos;
}
