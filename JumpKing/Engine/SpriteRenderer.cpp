#include "pch.h"
#include "framework.h"
#include "Engine/RenderContext.h"
#include "SpriteRenderer.h"

bool SpriteRenderer::Load(const std::wstring& texturePath, int32 row, int32 col)
{
    _isLoaded = _texture.Load(texturePath, row, col);

    _animIndexX = 0;
    _animIndexY = 0;
    _isPlaying = false;
    _isEnd = false;
    _sumTime = 0.0f;

    return _isLoaded;
}

void SpriteRenderer::Update(float deltaTime)
{
    if (!_isLoaded || !_isPlaying || _isEnd || _duration <= 0.0f)
    {
        return;
    }

    const int32 columnCount = _texture.GetColumnCount();
    const int32 rowCount = _texture.GetRowCount();
    const int32 totalCount =
        _fullFrame ? columnCount * rowCount : columnCount;

    if (totalCount <= 0)
    {
        return;
    }

    const float frameTime = _duration / static_cast<float>(totalCount);
    _sumTime += deltaTime;

    // 프레임이 잠시 느려져도 경과한 시간만큼 애니메이션을 따라잡습니다.
    while (_sumTime >= frameTime && !_isEnd)
    {
        _sumTime -= frameTime;

        const int32 currentIndex = _fullFrame ? _animIndexY * columnCount + _animIndexX : _animIndexX;
        int32 nextIndex = currentIndex + 1;

        if (nextIndex >= totalCount)
        {
            if (_loop)
            {
                nextIndex = 0;
            }
            else
            {
                _isEnd = true;
                _isPlaying = false;
                return;
            }
        }

        _animIndexX = nextIndex % columnCount;
        if (_fullFrame)
        {
            _animIndexY = nextIndex / columnCount;
        }
    }
}

void SpriteRenderer::Render(const RenderContext& context, const Vector2& ownerPosition)
{
    if (!_isLoaded)
    {
        return;
    }

    const D2D1_SIZE_F frameSize = _texture.GetFrameSize();
    const float sourceX = _animIndexX * frameSize.width;
    const float sourceY = _animIndexY * frameSize.height;
    const D2D1_RECT_F sourceRect = D2D1::RectF(
        sourceX,
        sourceY,
        sourceX + 32.0f,
        sourceY + 40.0f);

    _texture.Render(context, ownerPosition, sourceRect, _flipX);
}

void SpriteRenderer::ResetAnim(int32 row, bool loop, float duration)
{
    if (!_isLoaded || row < 0 || row >= _texture.GetRowCount() || duration <= 0.0f)
    {
        return;
    }

    _animIndexX = 0;
    _animIndexY = row;
    _isPlaying = true;
    _isEnd = false;
    _loop = loop;
    _duration = duration;
    _sumTime = 0.0f;
}

void SpriteRenderer::SetFrame(int32 row, int32 col)
{
    if (!_isLoaded || row < 0 || row >= _texture.GetRowCount() || col < 0 || col >= _texture.GetColumnCount())
    {
        return;
    }

    _animIndexX = col;
    _animIndexY = row;
    _isPlaying = false;
    _isEnd = false;
    _sumTime = 0.0f;
}
