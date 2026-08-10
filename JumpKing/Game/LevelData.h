#pragma once

#include "Engine/Types.h"
#include <d2d1.h>
#include <filesystem>
#include <string>
#include <vector>

enum class PlatformMaterial
{
    Land,
    Ice,
    Snow
};

struct PlatformData
{
    Rect bounds{};
    Vector2 slope{};
    bool hasSlope = false;
    bool support = false;
    PlatformMaterial material = PlatformMaterial::Land;
};

// slope는 박스의 어느 모서리가 solid(직각 코너)인지를 가리키는 부호 벡터다.
//
//   slope      solid 코너      대각선     의미
//   (+1,+1)    bottom-right    BL->TR     바닥 램프, 오른쪽으로 올라감
//   (-1,+1)    bottom-left     TL->BR     바닥 램프, 오른쪽으로 내려감
//   (-1,-1)    top-left        BL->TR     천장(처마), 오른쪽으로 갈수록 넓어짐
//   (+1,-1)    top-right       TL->BR     천장(처마), 오른쪽으로 갈수록 좁아짐
//
// isFloor       = slope.y > 0   (대각선 아래가 solid -> 위에 올라설 수 있음)
// isRisingRight = slope.x * slope.y > 0   (true면 BL-TR 대각선, false면 TL-BR 대각선)
bool IsSlopeFloor(const PlatformData& platform);
bool IsSlopeRisingRight(const PlatformData& platform);

// slope 대각선의 두 끝점을 world 좌표로 반환한다 (디버그 렌더링/충돌 판정 공용).
// outLeft = bounds.Left()에서의 점, outRight = bounds.Right()에서의 점.
void GetSlopeEndpoints(const PlatformData& platform, Vector2& outLeft, Vector2& outRight);

struct LevelLayers
{
    std::string background;
    std::string midground;
    std::string foreground;
};

struct LevelData
{
    int index = 0;
    LevelLayers layers;
    Vector2 sourceSize;
    Vector2 spawn;
    bool hasSpawn = false;
    std::vector<PlatformData> platforms;
};

bool LoadLevelData(
    const std::filesystem::path& jsonPath,
    const std::string& levelId,
    LevelData& output);

bool LoadAllLevelData(const filesystem::path& jsonPath, vector<LevelData>& output);