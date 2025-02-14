#ifndef ASSET_MANAGER_HPP
#define ASSET_MANAGER_HPP

#include <vector>
#include <unordered_map>

#include <raylib.h>

class AssetManager {
public:
    enum class TextureId {
        WallCornerTopLeft,
        WallCornerTopRight,
        WallCornerBottomLeft,
        WallCornerBottomRight,
        WallTerminationLeft,
        WallTerminationRight,
        WallTerminationUp,
        WallTerminationDown,
        WallCenter,
        WallHorizontal,
        WallVertical,
        HeadLeft,
        HeadRight,
        HeadUp,
        HeadDown,
        Food0,
        Food1,
        Food2,
        Food3,
        Food4,
        Food5,
        Nibble,
        LogoScreen
    };

    enum class FontId {
        Default
    };

    AssetManager();

    ~AssetManager();

    AssetManager(const AssetManager&) = delete;

    AssetManager& operator=(const AssetManager&) = delete;

    AssetManager(AssetManager&&) = delete;

    AssetManager& operator=(AssetManager&&) = delete;

    Texture get_texture(TextureId) const;

    Font get_font(FontId) const;

private:
    std::unordered_map<TextureId, Texture> m_textures;
    std::unordered_map<FontId, Font> m_fonts;
};

#endif // ASSET_MANAGER_HPP
