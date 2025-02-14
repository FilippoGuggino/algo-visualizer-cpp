#ifndef LOGO_SCENE_HPP
#define LOGO_SCENE_HPP

#include "scene.hpp"

#include <raylib.h>
#include "../settings.hpp"

class LogoScene : public Scene {
public:
    LogoScene(SharedContext& context);

    void draw() override;

    void handle_input() override;

    void update() override;

    void on_enter() override;

    void on_exit() override;

    void on_window_resized() override;

private:
    Texture m_texture;
    Font m_font;
    double m_start_time;
    Color m_background_color = Settings::DefaultBackgroundColor;
    Color m_text_color = BLACK;

    Camera2D m_camera = Camera2D { 0 };
};

#endif // LOGO_SCENE_HPP
