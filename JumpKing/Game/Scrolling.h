#pragma once

#include "Engine/Texture.h"
#include "Engine/SpriteRenderer.h"
#include "Game/LevelData.h"
#include <vector>

struct RenderContext;

class ScrollingClouds
{
public:
    bool Load(
        const std::vector<ScrollingCloudData>& cloudData,
        const std::vector<ScrollingBirdData>& birdData);
    void Update(float deltaTime);
    void Render(const RenderContext& context, ScrollLayer layer);

private:
    struct CloudInstance
    {
        Texture texture;
        Vector2 position;
        float speed = 0.0f;
        ScrollLayer layer = ScrollLayer::Background;
    };

    // Cloud와 달리 애니메이션되는 한 마리를 화면 밖으로 나가면
    // 반대편에서 다시 나타나게 그리는 스크롤 오브젝트.
    struct BirdInstance
    {
        SpriteRenderer sprite;
        Vector2 position;
        float speed = 0.0f;
        ScrollLayer layer = ScrollLayer::Background;
    };

    std::vector<CloudInstance> _clouds;
    std::vector<BirdInstance> _birds;
};
