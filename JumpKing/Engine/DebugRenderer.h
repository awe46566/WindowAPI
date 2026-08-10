#pragma once

#include "Engine/RenderContext.h"
#include "Engine/Types.h"

class DebugRenderer
{
public:
	static void DrawRect(const RenderContext& context, const Rect& rect, const D2D1_COLOR_F& color);
	static void DrawLine(const RenderContext& context, const Vector2& from, const Vector2& to, const D2D1_COLOR_F& color);
	static void ToggleColliderLine();
	static bool VisibleState();

private:
	static bool _showCollider;
};