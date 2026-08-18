#include "pch.h"
#include "HiddenWalls.h"
#include "Engine/ResourceCatalog.h"
#include "Engine/RenderContext.h"
#include "Engine/GameConstants.h"
#include "Framework/CollisionManager.h"

bool HiddenWalls::Load(const std::vector<PropData>& data)
{
    _hiddenWalls.clear();
    _hiddenWalls.reserve(data.size());

    for (const PropData& source : data)
    {
        const ImageResource* image = ResourceCatalog::GetInstance().FindImage(source.image);
        if (image == nullptr)
        {
            continue;
        }

        HiddenWallInstance instance;
        if (!instance.sprite.Load(image->path, image->rows, image->columns))
        {
            continue;
        }

        instance.sprite.SetFullFrame(true);
        instance.position = source.position;
        _hiddenWalls.push_back(std::move(instance));
    }

    return !_hiddenWalls.empty();
}

void HiddenWalls::Update(float deltaTime, const Rect& playerBounds)
{
    
    for (HiddenWallInstance& hiddenWall : _hiddenWalls)
    {
        const D2D1_SIZE_F frameSize = hiddenWall.sprite.GetFrameSize();

        Rect wallBounds
        {
            hiddenWall.position.x,
            hiddenWall.position.y,
            frameSize.width,
            frameSize.height
        };

        HitResult hit;
        bool overlapping = CollisionManager::GetInstance().CheckAABBToAABB(playerBounds, wallBounds, hit);
        
        if (overlapping)
        {
            hiddenWall.opacity -= GameConstants::HIDDEN_WALL_FADE_SPEED * deltaTime;
        }
        else
        {
            hiddenWall.opacity += GameConstants::HIDDEN_WALL_FADE_SPEED * deltaTime;
        }
        hiddenWall.opacity = clamp(hiddenWall.opacity, 0.0f, 1.0f);

        hiddenWall.sprite.SetOpacity(hiddenWall.opacity);
    }
    
}

void HiddenWalls::Render(const RenderContext& context)
{
    for (HiddenWallInstance& hiddenWall : _hiddenWalls)
    {
        hiddenWall.sprite.Render(context, hiddenWall.position);
    }
}
