#include "pch.h"
#include "Scrolling.h"
#include "Engine/ResourceCatalog.h"
#include "Engine/RenderContext.h"
#include "Engine/GameConstants.h"
using namespace GameConstants;

bool ScrollingClouds::Load(
    const std::vector<ScrollingCloudData>& cloudData,
    const std::vector<ScrollingBirdData>& birdData)
{
    _clouds.clear();
    _clouds.reserve(cloudData.size());

    for (const ScrollingCloudData& source : cloudData)
    {
        const ImageResource* image = ResourceCatalog::GetInstance().FindImage(source.image);
        if (image == nullptr)
        {
            continue;
        }

        CloudInstance instance;
        if (!instance.texture.Load(image->path, image->rows, image->columns))
        {
            continue;
        }

        instance.position = source.position;
        instance.speed = source.speed;
        instance.layer = source.layer;
        _clouds.push_back(std::move(instance));
    }

    _birds.clear();
    _birds.reserve(birdData.size());

    for (const ScrollingBirdData& source : birdData)
    {
        const ImageResource* image = ResourceCatalog::GetInstance().FindImage(source.image);
        if (image == nullptr)
        {
            continue;
        }

        BirdInstance instance;
        if (!instance.sprite.Load(image->path, image->rows, image->columns))
        {
            continue;
        }

        // 스프라이트 시트 전체(2행 x 4열 = 8프레임)를 순서대로 순환 재생합니다.
        instance.sprite.SetFullFrame(true);
        instance.sprite.ResetAnim(0, true, BIRD_ANIM_DURATION, 0);

        instance.position = source.position;
        instance.speed = source.speed;
        instance.layer = source.layer;
        _birds.push_back(std::move(instance));
    }

    return !_clouds.empty() || !_birds.empty();
}

void ScrollingClouds::Update(float deltaTime)
{
    for (CloudInstance& cloud : _clouds)
    {
        cloud.position.x += cloud.speed * deltaTime;

        const float width = static_cast<float>(cloud.texture.GetWidth());
        if (cloud.position.x > SCREEN_WIDTH)
        {
            cloud.position.x -= width;
        }
        else if (cloud.position.x < -width)
        {
            cloud.position.x += width;
        }
    }

    for (BirdInstance& bird : _birds)
    {
        bird.sprite.Update(deltaTime);
        bird.position.x += bird.speed * deltaTime;

        const float width = bird.sprite.GetFrameSize().width;
        if (bird.position.x > SCREEN_WIDTH)
        {
            bird.position.x = -width;
        }
        else if (bird.position.x < -width)
        {
            bird.position.x = static_cast<float>(SCREEN_WIDTH);
        }
    }
}

void ScrollingClouds::Render(const RenderContext& context, ScrollLayer layer)
{
    for (CloudInstance& cloud : _clouds)
    {
        if (cloud.layer != layer)
        {
            continue;
        }

        const float width = static_cast<float>(cloud.texture.GetWidth());
        const float height = static_cast<float>(cloud.texture.GetHeight());
        const D2D1_RECT_F sourceRect = D2D1::RectF(0.0f, 0.0f, width, height);

        cloud.texture.Render(context, cloud.position, sourceRect);
        cloud.texture.Render(context, Vector2{ cloud.position.x - width, cloud.position.y }, sourceRect);
        cloud.texture.Render(context, Vector2{ cloud.position.x + width, cloud.position.y }, sourceRect);
    }

    for (BirdInstance& bird : _birds)
    {
        if (bird.layer != layer)
        {
            continue;
        }

        bird.sprite.Render(context, bird.position);
    }
}
