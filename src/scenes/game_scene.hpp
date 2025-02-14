#ifndef GAME_SCENE_HPP
#define GAME_SCENE_HPP

#include "scene.hpp"

#include <raylib.h>

class GameScene : public Scene {
public:
    GameScene(SharedContext& context);

    void draw() override;

    void handle_input() override;

    void update() override;

    void on_enter() override;

    void on_exit() override;

    void on_window_resized() override
    {
    }
};

#endif // GAME_SCENE_HPP
