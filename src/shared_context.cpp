#include "shared_context.hpp"

#include <array>

#include "scenes/game_scene.hpp"
#include "scenes/logo_scene.hpp"
#include "scenes/menu_scene.hpp"

SharedContext::SharedContext()
{
    m_logo_scene = std::make_unique<LogoScene>(*this);
    m_game_scene = std::make_unique<GameScene>(*this);
    m_menu_scene = std::make_unique<MenuScene>(*this);

    m_current_scene = m_logo_scene.get();
}

Scene* SharedContext::current_scene() const { return m_current_scene; }

void SharedContext::next()
{
    static int index = 0;

    m_current_scene->on_exit();

    index++;

    std::array<Scene*, 3> scenes = {
        m_logo_scene.get(),
        m_game_scene.get(),
        m_menu_scene.get()
    };

    m_current_scene = scenes[index % scenes.size()];

    m_current_scene->on_enter();
}
