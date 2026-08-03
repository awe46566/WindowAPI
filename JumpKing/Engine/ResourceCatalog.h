#pragma once

#include "Engine/Singleton.h"
#include <filesystem>
#include <string>
#include <unordered_map>

struct ImageResource
{
    std::filesystem::path path;
    int rows = 1;
    int columns = 1;
};

class ResourceCatalog : public Singleton<ResourceCatalog>
{
    friend class Singleton<ResourceCatalog>;

public:
    bool Load(const std::filesystem::path& jsonPath);
    const ImageResource* FindImage(const std::string& id) const;

private:
    ResourceCatalog() = default;
    std::unordered_map<std::string, ImageResource> _images;
};
