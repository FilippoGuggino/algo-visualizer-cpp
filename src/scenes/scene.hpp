#ifndef SCENE_HPP
#define SCENE_HPP

class SharedContext;

class Scene {
public:
    Scene(SharedContext& context)
        : m_context(context)
    {
    }

    virtual ~Scene() = default;
    virtual void draw() = 0;
    virtual void handle_input() = 0;
    virtual void update() = 0;
    virtual void on_enter() = 0;
    virtual void on_exit() = 0;
    virtual void on_window_resized() = 0;

protected:
    SharedContext& m_context;
};

#endif // SCENE_HPP
