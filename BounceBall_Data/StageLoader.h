#pragma once

// 여기서만 코드를 수정할경우
// EditorScene, GameScene 둘다 동일한 코드를 적용받게 된다.
// 유지보수를 위해서
class StageLoader
{
public:
	// 반환값을 해당 스테이지에서 획득해야하는 전체 개수
	int32 Load(class Scene* scene, json root);
};

