#include "pch.h"
#include "UIPanel.h"
#include "Widget.h"

UIPanel::~UIPanel()
{
	for (Widget* widget : _widgets)
	{
		delete widget;
	}
	_widgets.clear();
}

void UIPanel::AddWidget(Widget* widget)
{
	if (nullptr == widget)
		return;

	_widgets.push_back(widget);
}

void UIPanel::Update(float deltaTime)
{
	if (false == _visible)
		return;

	for (Widget* widget : _widgets)
	{
		widget->Update(deltaTime);
	}
}

void UIPanel::Render(HDC hdc)
{
	if (false == _visible)
		return;

	for (Widget* widget : _widgets)
	{
		widget->Render(hdc);
	}
}
