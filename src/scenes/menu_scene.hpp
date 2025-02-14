#ifndef MENU_SCENE_HPP
#define MENU_SCENE_HPP

#include "scene.hpp"

#include <raylib.h>

class MenuScene : public Scene {
public:
    MenuScene(SharedContext& context);

    void draw() override;

    void handle_input() override;

    void update() override;

    void on_enter() override;

    void on_exit() override;

    void on_window_resized() override
    {
    }
};

#endif // MENU_SCENE_HPP
