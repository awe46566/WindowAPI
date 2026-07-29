#pragma once

float RadianToDegree(float radian);
float DegreeToRadian(float degree);

enum class RenderLayer
{
	// 아래 순서대로 렌더링이 실행된다.
	Background, // 제일 아래
	Enemy,
	Bullet,
	Player,
	Effect,     // 제일 위
	
	// 최대 개수
	Count
};

// Actor를 판단할수있는 식별자 Type
enum class ActorType
{
	Background,
	Block,
	Ball,
	Star,
	Effect,
	Player,
	Count,
};

enum class SceneType
{
	Lobby,
	Game,
	Editor,
	AStar,
	Max,
};


enum class ColliderType
{
	Circle,
	AABB,
	Max
};

enum FontSize
{
	FONT_12 = 12,
	FONT_20 = 20,
	FONT_30 = 30,
};

enum DirType
{
	DIR_UP,
	DIR_DOWN,
	DIR_LEFT,
	DIR_RIGHT,
};