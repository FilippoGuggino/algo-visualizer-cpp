#ifndef SHARED_CONTEXT_HPP
#define SHARED_CONTEXT_HPP

#include <memory>
#include "asset_manager.hpp"

class Scene;

class SharedContext {
public:
    SharedContext();

    void next();

    Scene* current_scene() const;

    const AssetManager& asset_manager() const { return m_manager; }

private:
    AssetManager m_manager;

    Scene* m_current_scene;

    std::unique_ptr<Scene> m_logo_scene;
    std::unique_ptr<Scene> m_game_scene;
    std::unique_ptr<Scene> m_menu_scene;
};

#endif // SHARED_CONTEXT_HPP
