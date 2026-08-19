#include "pch.h"
#include "Props.h"
#include "Engine/ResourceCatalog.h"
#include "Engine/RenderContext.h"

bool Props::Load(const std::vector<PropData>& data)
{
    _props.clear();
    _props.reserve(data.size());

    for (const PropData& source : data)
    {
        const ImageResource* image = ResourceCatalog::GetInstance().FindImage(source.image);
        if (image == nullptr)
        {
            continue;
        }

        PropInstance instance;
        if (!instance.sprite.Load(image->path, image->rows, image->columns))
        {
            continue;
        }

        instance.sprite.SetFullFrame(true);
        instance.sprite.setFlipX(source.flipX);

        const int32 frameCount = image->rows * image->columns;
        instance.sprite.ResetAnim(0, true, source.frameInterval * frameCount, 0);

        instance.position = source.position;
        _props.push_back(std::move(instance));
    }

    return !_props.empty();
}

void Props::Update(float deltaTime)
{
    for (PropInstance& prop : _props)
    {
        prop.sprite.Update(deltaTime);
    }
}

void Props::Render(const RenderContext& context)
{
    for (PropInstance& prop : _props)
    {
        prop.sprite.Render(context, prop.position);
    }
}
