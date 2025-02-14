#include "menu_scene.hpp"

#include <iostream>

#include "../shared_context.hpp"

const Color kBackgroundColor = GREEN;

MenuScene::MenuScene(SharedContext& context)
    : Scene(context)
{
}

void MenuScene::draw()
{
    BeginDrawing();

    ClearBackground(kBackgroundColor);

    EndDrawing();
}

void MenuScene::handle_input()
{
    if (IsKeyPressed(KEY_RIGHT)) {
        m_context.next();
    }
}

void MenuScene::update()
{
}

void MenuScene::on_enter()
{
    std::cout << "MenuScene::on_enter" << '\n';
}

void MenuScene::on_exit()
{
    std::cout << "MenuScene::on_exit" << '\n';
}
