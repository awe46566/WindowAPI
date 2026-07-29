#include "pch.h"
#include "DebugAStar.h"
#include "InputManager.h"
#include "ResourceManager.h"
#include "Game.h"

void DebugAStar::Update(float deltaTime)
{
	if (InputManager::GetInstance().GetButtonDown(KeyType::F1))
	{
		_drawDebug = !_drawDebug;
	}
}

void DebugAStar::Render(HDC renderTarget)
{
	if (_drawDebug)
	{
		HPEN pen = ResourceManager::GetInstance().GetRedPen();
		HFONT font = ResourceManager::GetInstance().GetFont(FontSize::FONT_12);

		HPEN oldPen = (HPEN)::SelectObject(renderTarget, pen);
		HFONT oldFont = (HFONT)::SelectObject(renderTarget, font);
		::SetTextColor(renderTarget, RGB(255, 0, 0));
		::SetBkMode(renderTarget, TRANSPARENT);

		for (auto iter : best)
		{
			Cell pos = iter.first;
			int32 cost = iter.second;

			Vector screenPos;
			screenPos.x = (float)pos.iX * BLOCK_SIZE;
			screenPos.y = (float)pos.iY * BLOCK_SIZE;

			int32 g = -1;
			int32 h = -1;
			auto find = openList.find(pos);
			if (find != openList.end())
			{
				g = find->second.g;
				h = find->second.h;
			}

			wstring str = std::format(L"f:{0}\ng:{1},h:{2}", cost, g, h);

			RECT rect = {
				(LONG)screenPos.x,
				(LONG)screenPos.y,
				(LONG)(screenPos.x + BLOCK_SIZE),
				(LONG)(screenPos.y + BLOCK_SIZE)
			};
			::DrawTextW(renderTarget, str.c_str(), (int)str.size(), &rect, DT_LEFT | DT_TOP);
		}

		::SelectObject(renderTarget, oldPen);
		::SelectObject(renderTarget, oldFont);
	}
}
