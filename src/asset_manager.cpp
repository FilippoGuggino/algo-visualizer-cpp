#include "asset_manager.hpp"

#include <string>
#include <unordered_map>

static const std::unordered_map<AssetManager::TextureId, std::string> textures = {
    { AssetManager::TextureId::WallCornerTopLeft, "assets/sprites/corner_tl.png" },
    { AssetManager::TextureId::WallCornerTopRight, "assets/sprites/corner_tr.png" },
    { AssetManager::TextureId::WallCornerBottomLeft, "assets/sprites/corner_bl.png" },
    { AssetManager::TextureId::WallCornerBottomRight, "assets/sprites/corner_br.png" },
    { AssetManager::TextureId::WallTerminationLeft, "assets/sprites/termination_l.png" },
    { AssetManager::TextureId::WallTerminationRight, "assets/sprites/termination_r.png" },
    { AssetManager::TextureId::WallTerminationUp, "assets/sprites/termination_u.png" },
    { AssetManager::TextureId::WallTerminationDown, "assets/sprites/termination_d.png" },
    { AssetManager::TextureId::WallCenter, "assets/sprites/wall_c.png" },
    { AssetManager::TextureId::WallHorizontal, "assets/sprites/wallh.png" },
    { AssetManager::TextureId::WallVertical, "assets/sprites/wallv.png" },
    { AssetManager::TextureId::HeadLeft, "assets/sprites/headl.png" },
    { AssetManager::TextureId::HeadRight, "assets/sprites/headr.png" },
    { AssetManager::TextureId::HeadUp, "assets/sprites/headu.png" },
    { AssetManager::TextureId::HeadDown, "assets/sprites/headd.png" },
    { AssetManager::TextureId::Food0, "assets/sprites/food0.png" },
    { AssetManager::TextureId::Food1, "assets/sprites/food1.png" },
    { AssetManager::TextureId::Food2, "assets/sprites/food2.png" },
    { AssetManager::TextureId::Food3, "assets/sprites/food3.png" },
    { AssetManager::TextureId::Food4, "assets/sprites/food4.png" },
    { AssetManager::TextureId::Food5, "assets/sprites/food5.png" },
    { AssetManager::TextureId::Nibble, "assets/sprites/nibble.png" },
    { AssetManager::TextureId::LogoScreen, "assets/logo.png" },
};

static const std::unordered_map<AssetManager::FontId, std::string> fonts = {
    { AssetManager::FontId::Default, "assets/fonts/nokiafc22.ttf" }
};

AssetManager::AssetManager()
{
    for (const auto& [key, path] : textures) {
        // TODO: check for file exists or errors
        m_textures.emplace(key, LoadTexture(path.c_str()));
    }

    for (const auto& [key, path] : fonts) {
        // TODO: check for file exists or errors
        m_fonts.emplace(key, LoadFontEx(path.c_str(), 64, NULL, 0));
    }
}

AssetManager::~AssetManager()
{
    for (auto [_, texture] : m_textures) {
        UnloadTexture(texture);
    }

    for (auto [_, font] : m_fonts) {
        UnloadFont(font);
    }
}

template <typename T>
T find(const auto& map, const auto& id)
{
    auto it = map.find(id);

    if (it == std::end(map)) {
        return T();
    }

    return it->second;
}

Texture AssetManager::get_texture(TextureId id) const
{
    return find<Texture>(m_textures, id);
}

Font AssetManager::get_font(FontId id) const
{
    return find<Font>(m_fonts, id);
}
