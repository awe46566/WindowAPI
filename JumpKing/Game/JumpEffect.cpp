#include "pch.h"
#include "JumpEffect.h"
#include "Engine/ResourceCatalog.h"
#include "Engine/RenderContext.h"
#include "Engine/GameConstants.h"
using namespace GameConstants;

bool JumpEffect::Load()
{  
    const ImageResource* normalImage = ResourceCatalog::GetInstance().FindImage("particle_jump");
    if (normalImage != nullptr)
    {
        _normalSprite.Load(normalImage->path, normalImage->rows, normalImage->columns);
    }

    const ImageResource* snowImage = ResourceCatalog::GetInstance().FindImage("particle_jump_snow");
    if (snowImage != nullptr)
    {
        _snowSprite.Load(snowImage->path, snowImage->rows, snowImage->columns);
    }

    return normalImage != nullptr && snowImage != nullptr;
}

void JumpEffect::Play(const Vector2& position, PlatformMaterial material)
{
    if (material == PlatformMaterial::Snow)
    {
        _activeSprite = &_snowSprite;
    }      
    else
    {
        _activeSprite = &_normalSprite;
    }        

    _position = position;
    _activeSprite->ResetAnim(0, false, PARTICLE_JUMP_ANIM_DURATION, 0);
}

void JumpEffect::Update(float deltaTime)
{
    // TODO: _activeSprite가 있으면 Update(deltaTime) 호출.
    if (_activeSprite != nullptr)
    {
        _activeSprite->Update(deltaTime);
    }
}

void JumpEffect::Render(const RenderContext& context)
{
    // TODO: _activeSprite가 있고 아직 끝나지 않았으면(!_activeSprite->IsEnd()) Render(context, _position) 호출.
    // SpriteRenderer::Render는 IsEnd() 여부와 상관없이 항상 그리므로, 재생이 끝난 뒤엔 여기서 걸러줘야 함.
    if (_activeSprite != nullptr && !_activeSprite->IsEnd())
    {
        _activeSprite->Render(context, _position);
    }
}
