#pragma once

class IPickupable
{
public:
    virtual ~IPickupable() = default;

    virtual void OnPickedUp(class Ball* ball) = 0;
};