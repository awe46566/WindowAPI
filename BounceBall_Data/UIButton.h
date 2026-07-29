#pragma once

#include "Widget.h"

class UIButton : public Widget
{
public:
	virtual void Update(float deltaTime) override;

	void SetOnClick(std::function<void()> onClick)
	{
		_onClick = onClick;
	}

private:
	// 버튼이 클릭되었을때 해야하는 일들을 수행 : 함수포인터로 만들면 결합도가 낮아진다.
	std::function<void()> _onClick;
};