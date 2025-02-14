#include <raylib.h>

#include "settings.hpp"
#include "scenes/scene.hpp"
#include "shared_context.hpp"

int main(void)
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    InitWindow(Settings::DefaultScreenWidth, Settings::DefaultScreenHeight, "Snake II");

    {
        SetTargetFPS(25);

        SharedContext context;

        context.current_scene()->on_enter();

        while (!WindowShouldClose()) {
            context.current_scene()->handle_input();
            context.current_scene()->update();

            if (IsWindowResized()) {
                context.current_scene()->on_window_resized();
            }

            context.current_scene()->draw();
        }

        context.current_scene()->on_exit();
    }

    CloseWindow();

    return 0;
}
