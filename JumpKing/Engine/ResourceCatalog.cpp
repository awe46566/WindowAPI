#include "pch.h"
#include "Engine/ResourceCatalog.h"
#include "../../BounceBall_Data/Json/nlohmann/json.hpp"

bool ResourceCatalog::Load(const std::filesystem::path& jsonPath)
{
    std::ifstream input(jsonPath);
    if (!input)
    {
        return false;
    }

    try
    {
        const nlohmann::json document = nlohmann::json::parse(input);
        const std::filesystem::path projectRoot = jsonPath.parent_path().parent_path();

        _images.clear();
        for (const auto& [id, value] : document.at("images").items())
        {
            ImageResource resource;
            resource.path = (projectRoot / value.at("path").get<std::string>()).lexically_normal();
            resource.rows = value.value("rows", 1);
            resource.columns = value.value("columns", 1);
            _images.emplace(id, std::move(resource));
        }
    }
    catch (const nlohmann::json::exception&)
    {
        _images.clear();
        return false;
    }

    return true;
}

const ImageResource* ResourceCatalog::FindImage(const std::string& id) const
{
    const auto found = _images.find(id);
    return found == _images.end() ? nullptr : &found->second;
}
