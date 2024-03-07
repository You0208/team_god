#pragma once

#include <map>
#include <string>
#include <memory>

#include "../Model/Model.h"
#include "../Graphics/Sprite.h"


class ResourceManager
{
private:
    ResourceManager() {}
    ~ResourceManager() {}

public:
    static ResourceManager& Instance()
    {
        static ResourceManager instance;
        return instance;
    }

    // モデルリソース読み込み
    const std::shared_ptr<SkinnedMesh> LoadModelResource(
        ID3D11Device* device,
        const char* const      fbx_filename,
        const bool             triangulate = false,
        const float            sampling_rate = 0
    );

    // スプライトリソース読み込み
    const std::shared_ptr<Sprite> load_sprite_resource(
        ID3D11Device* device,
        const wchar_t* const      filename
    );

    // モデルリソース読み込み(アニメーション)
    const std::shared_ptr<SkinnedMesh> LoadModelResource(
        ID3D11Device* device,
        const char* const                  fbx_filename,
        std::vector<std::string> animation_filenames,
        const bool                         triangulate = false,
        const float                        sampling_rate = 0
    );

private:
    using ModelMap = std::map<const char*, std::weak_ptr<SkinnedMesh>>;
    using sprite_map = std::map<const char*, std::weak_ptr<Sprite>>;

private:
    ModelMap models = {};
    sprite_map sprites = {};

};

