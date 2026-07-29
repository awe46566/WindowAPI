#pragma once
#include "Actor.h"

class Ball : public Actor
{
	using Super = Actor;
public:
	virtual void Init() override;
	virtual void Update(float deltaTime) override;

	virtual RenderLayer GetRenderLayer() override { return RenderLayer::Background; }
	virtual ActorType GetActorType() override { return ActorType::Ball; }
	
	virtual void OnEnter(Actor* other, const HitResult& hit) override;
	virtual void OnStay(Actor* other, const HitResult& hit) override;
	virtual void OnExit(Actor* other) override;

	void SetEditorScene() { _editorScene = true; }

private:
	bool _editorScene = false;
	Vector _velocity;		// 위치가 변화되는 변화량 (속도 개념)
	float  _gravity = 9.8f * 150;	// 중력

	float _bounceSpeedY = -650.f;   // 반사 후 고정 수직 속도
	float _bounceSpeedX = 300.f;    // 반사 후 고정 수직 속도
	float _sideForce = 700.0f;        // 좌우로 이동시 힘의 크기
	float _mass = 1.0f;               // 질량 (고정값)
	float _sideFriction = 4.0f;       // 수평 속도 감쇠(마찰) 계수 (클수록 빨리 멈춤)
};

