#include "pch.h"
#include "ColliderAABB.h"
#include "Actor.h"

void ColliderAABB::Init(Actor* owner, int32 width, int32 height)
{
    _owner = owner;
    _width = width;
    _height = height;
}

Vector ColliderAABB::GetMin()
{
    Vector pos = _owner->GetPos();
    return Vector(pos.x - _width * 0.5f, pos.y - _height * 0.5f);
}

Vector ColliderAABB::GetMax()
{
    Vector pos = _owner->GetPos();
    return Vector(pos.x + _width * 0.5f, pos.y + _height * 0.5f);
}
