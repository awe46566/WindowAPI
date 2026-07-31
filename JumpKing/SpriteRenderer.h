#pragma once

#include "Framework/Component.h"
#include "Texture.h"

#include <string>

class SpriteRenderer : public Component
{
public:
    bool Load(
        const std::wstring& texturePath,
        int32 row = 1,
        int32 col = 1);

    void Update(float deltaTime) override;
    void Render(
        const RenderContext& context,
        const Vector2& ownerPosition) override;

    // row에 있는 프레임을 duration초 동안 재생합니다.
    void ResetAnim(int32 row, bool loop, float duration);
    void SetFrame(int32 row, int32 col);
    void SetFullFrame(bool fullFrame) { _fullFrame = fullFrame; }

    bool IsEnd() const { return _isEnd; }

private:
    Texture _texture;
    bool _isLoaded = false;

    int32 _animIndexX = 0;
    int32 _animIndexY = 0;

    bool _isPlaying = false;
    bool _isEnd = false;
    bool _loop = false;
    bool _fullFrame = false;

    float _duration = 0.0f;
    float _sumTime = 0.0f;
};
