#pragma once

class Wind
{
public:
    void Update(float deltaTime);

    float GetForce() const { return _force; }
    float GetScrollOffset() const { return _scrollOffset; }

private:
    float _phase = 0.0f;
    float _force = 0.0f;
    float _scrollOffset = 0.0f;
};
