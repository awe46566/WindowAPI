#include "pch.h"
#include "DebugRenderer.h"

bool DebugRenderer::_showCollider = false;
void DebugRenderer::DrawRect(const RenderContext& context, const Rect& rect, const D2D1_COLOR_F& color)
{
	if (!_showCollider)
		return;

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

void DebugRenderer::DrawLine(const RenderContext& context, const Vector2& from, const Vector2& to, const D2D1_COLOR_F& color)
{
	if (!_showCollider)
		return;

	if (context.target == nullptr || context.defaultBrush == nullptr)
		return;

	const D2D1_COLOR_F previousColor =
		context.defaultBrush->GetColor();

	context.defaultBrush->SetColor(color);

	context.target->DrawLine(
		D2D1::Point2F(from.x, from.y),
		D2D1::Point2F(to.x, to.y),
		context.defaultBrush,
		1.0f);

	context.defaultBrush->SetColor(previousColor);
}

void DebugRenderer::ToggleColliderLine()
{
	_showCollider = !_showCollider;
}

bool DebugRenderer::VisibleState()
{
	return _showCollider;
}


