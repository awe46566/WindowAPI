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
    void Render(const RenderContext& context, const Vector2& ownerPosition) override;

    // row에 있는 프레임을 duration초 동안 재생합니다.
    // frameCount가 0 이하이면 해당 row의 전체 컬럼 수를 그대로 사용합니다.
    void ResetAnim(int32 row, bool loop, float duration, int32 frameCount = 0);
    void SetFrame(int32 row, int32 col);
    void SetFullFrame(bool fullFrame) { _fullFrame = fullFrame; }
    void setFlipX(bool flipX) { _flipX = flipX; }
    void SetOpacity(float opacity) { _opacity = opacity; }
    float GetOpacity() const { return _opacity; }

    bool IsEnd() const { return _isEnd; }
    D2D1_SIZE_F GetFrameSize() const { return _texture.GetFrameSize(); }

private:
    Texture _texture;
    bool _isLoaded = false;

    int32 _animIndexX = 0;
    int32 _animIndexY = 0;

    bool _isPlaying = false;
    bool _isEnd = false;
    bool _loop = false;
    bool _fullFrame = false;
    bool _flipX = false;
    float _opacity = 1.0f;

    float _duration = 0.0f;
    float _sumTime = 0.0f;
    int32 _frameCount = 0;
};
