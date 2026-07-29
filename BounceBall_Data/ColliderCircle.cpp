#include "pch.h"
#include "ColliderCircle.h"
#include "Actor.h"
#include "Game.h"
#include "Scene.h"

void ColliderCircle::Init(Actor* owner, int radius)
{
    _owner = owner;
    _radius = radius;
}

void ColliderCircle::Render(HDC hdc, Vector pos)
{
    if (nullptr == _owner)
        return;

    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0)); // 빨간색 펜 생성
    HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH); // 투명 브러시 사용

    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

    // 원의 중심과 반지름 설정
    Vector screenPos = _owner->GetPos();

    int32 left = (int32)(screenPos.x - _radius);
    int32 top = (int32)(screenPos.y - _radius);
    int32 right = (int32)(screenPos.x + _radius);
    int32 bottom = (int32)(screenPos.y + _radius);

    Ellipse(hdc, left, top, right, bottom); // 원 그리기

    // 이전 GDI 객체 복원
    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);

    // 생성한 GDI 객체 삭제
    DeleteObject(hPen);
}
