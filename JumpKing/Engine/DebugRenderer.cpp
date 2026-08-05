#include "pch.h"
#include "DebugRenderer.h"

void DebugRenderer::DrawRect(const RenderContext& context, const Rect& rect, const D2D1_COLOR_F& color)
{
	if (context.target == nullptr || context.defaultBrush == nullptr)
		return;

	const D2D1_COLOR_F previousColor =
		context.defaultBrush->GetColor();

	context.defaultBrush->SetColor(color);

	const D2D1_RECT_F drawRect = D2D1::RectF(
		rect.Left(),
		rect.Top(),
		rect.Right(),
		rect.Bottom());

	context.target->DrawRectangle(
		drawRect,
		context.defaultBrush,
		1.0f);

	context.defaultBrush->SetColor(previousColor);
}
