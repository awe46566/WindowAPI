#include "pch.h"
#include "Game/LevelData.h"
#include "../../BounceBall_Data/Json/nlohmann/json.hpp"

bool LoadLevelData(
    const std::filesystem::path& jsonPath,
    const std::string& levelId,
    LevelData& output)
{
    std::ifstream input(jsonPath);
    if (!input)
    {
        return false;
    }

    try
    {
        const nlohmann::json document = nlohmann::json::parse(input);
        const nlohmann::json& level = document.at("levels").at(levelId);

        LevelData loaded;
        loaded.index = level.at("index").get<int>();

        const nlohmann::json& layers = level.at("layers");
        loaded.layers.background = layers.value("background", "");
        loaded.layers.midground = layers.at("midground").get<std::string>();
        loaded.layers.foreground = layers.value("foreground", "");

        loaded.sourceSize =
        {
            level.at("size").at("width").get<float>(),
            level.at("size").at("height").get<float>()
        };

        if (level.contains("spawn"))
        {
            loaded.spawn =
            {
                level.at("spawn").at("x").get<float>(),
                level.at("spawn").at("y").get<float>()
            };
            loaded.hasSpawn = true;
        }

        for (const nlohmann::json& platform : level.at("platforms"))
        {
            const float x = platform.at("x").get<float>();
            const float y = platform.at("y").get<float>();
            const float width = platform.at("width").get<float>();
            const float height = platform.at("height").get<float>();

            PlatformData data;
            data.bounds = { x, y, x + width, y + height };
            data.support = platform.value("support", false);

            if (!platform.at("slope").is_null())
            {
                data.hasSlope = true;
                data.slope =
                {
                    platform.at("slope").at(0).get<float>(),
                    platform.at("slope").at(1).get<float>()
                };
            }

            const std::string material = platform.value("material", std::string("land"));
            data.material =
                material == "ice" ? PlatformMaterial::Ice :
                material == "snow" ? PlatformMaterial::Snow :
                PlatformMaterial::Land;
            loaded.platforms.push_back(data);
        }

        if (loaded.sourceSize.x <= 0.0f || loaded.sourceSize.y <= 0.0f)
        {
            return false;
        }

        output = std::move(loaded);
        return true;
    }
    catch (const nlohmann::json::exception&)
    {
        return false;
    }
}
