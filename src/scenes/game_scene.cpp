#include "game_scene.hpp"

#include <iostream>

#include "../shared_context.hpp"

const Color kBackgroundColor = BLUE;

GameScene::GameScene(SharedContext& context)
    : Scene(context)
{
}

void GameScene::draw()
{
    BeginDrawing();

    ClearBackground(kBackgroundColor);

    EndDrawing();
}

void GameScene::handle_input()
{
    if (IsKeyPressed(KEY_RIGHT)) {
        m_context.next();
    }
}

void GameScene::update()
{
}

void GameScene::on_enter()
{
    std::cout << "GameScene::on_enter" << '\n';
}

void GameScene::on_exit()
{
    std::cout << "GameScene::on_exit" << '\n';
}
