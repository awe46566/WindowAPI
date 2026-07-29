#pragma once

#include "DataObject.h"

// 적의 속성들을 정의하는 데이터 테이블
class EnemyData : public DataObject
{
public:
	struct Item // 일일히 struct 이름 짓기가 너무 귀찮아서, 모두 Item 으로 통일하자.
	{

	};
};

class ResourceData : public DataObject
{
public:
	struct Item
	{
		wstring key;
		wstring fileName;
		int32 transparent = -1;
		int32 countX = 1;
		int32 countY = 1;
		bool loop = false;
		bool flip = false;	// 좌우반전 필요한 텍스처인지
		float dur = 1.0f;
	};
	struct SoundItem
	{
		wstring key;
		wstring fileName;
		bool loop;
	};

public:
	virtual wstring GetFileName() override { return L"ResourceData.json"; }
	virtual void Load(const json& data) override;

private:
	void loadItem(const json& table, SceneType type);
	void loadSoundItem(const json& table);

public:
	unordered_map<wstring, Item> _sceneData[(int32)SceneType::Max];

	unordered_map<wstring, SoundItem> _soundData;
};

