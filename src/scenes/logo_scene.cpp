#include "logo_scene.hpp"

#include <iostream>

#include "../shared_context.hpp"

LogoScene::LogoScene(SharedContext& context)
    : Scene(context)
{
    m_texture = context.asset_manager().get_texture(AssetManager::TextureId::LogoScreen);
    m_font = context.asset_manager().get_font(AssetManager::FontId::Default);
}

void LogoScene::draw()
{
    BeginDrawing();

    ClearBackground(Settings::DefaultBackgroundColor);

    BeginMode2D(m_camera);
    {
        DrawRectangleLines(0, 0, Settings::NokiaScreenWidth, Settings::NokiaScreenHeight, BLACK);
        DrawTexture(m_texture, (Settings::NokiaScreenWidth - m_texture.width) / 2.0f, 0, WHITE);

        const char text[] = "Press any key to start...";
        const float font_size = 16.0f;
        const float font_spacing = 0.0f;

        Vector2 text_size = MeasureTextEx(m_font, text, font_size, font_spacing);

        Vector2 text_position = {
            (Settings::NokiaScreenWidth - text_size.x) / 2.0f,
            1.2f * m_texture.height
        };

        DrawRectangle(
            text_position.x - (1.1f * text_size.x - text_size.x) / 2,
            text_position.y - (1.1f * text_size.y - text_size.y) / 2,
            1.1f * text_size.x,
            1.1f * text_size.y,
            m_background_color);

        DrawTextEx(m_font, text, text_position, font_size, font_spacing, m_text_color);
    }
    EndMode2D();

    EndDrawing();
}

void LogoScene::handle_input()
{
    // check for any key pressed
    if (GetKeyPressed() != KEY_NULL) {
        m_context.next();
    }
}

void LogoScene::update()
{
    if (GetTime() - m_start_time > 0.5) {
        std::swap(m_background_color, m_text_color);
        m_start_time = GetTime();
    }
}

void LogoScene::on_window_resized()
{
    m_camera.offset = (Vector2) { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
    m_camera.zoom = Settings::DefaultCameraZoom * std::min(GetScreenWidth() / float(Settings::DefaultScreenWidth), GetScreenHeight() / float(Settings::DefaultScreenHeight));
}

void LogoScene::on_enter()
{
    std::cout << "LogoScene::on_enter" << '\n';

    m_start_time = GetTime();

    m_background_color = Settings::DefaultBackgroundColor;
    m_text_color = BLACK;

    m_camera = { 0 };
    m_camera.target = (Vector2) { Settings::NokiaScreenWidth / 2.0f, Settings::NokiaScreenHeight / 2.0f };
    m_camera.offset = (Vector2) { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
    m_camera.rotation = 0.0f;
    m_camera.zoom = Settings::DefaultCameraZoom;
}

void LogoScene::on_exit()
{
    std::cout << "LogoScene::on_exit" << '\n';
}
