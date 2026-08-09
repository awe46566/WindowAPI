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