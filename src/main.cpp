#include <raylib.h>

int main(void)
{
    InitWindow(1280, 720, "WebAssembly");

    Font font = LoadFont("resources/fonts/nokiafc22.ttf");

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawTextEx(font, "Hello, world", Vector2 { 190, 200 }, 20, 2, LIGHTGRAY);

        EndDrawing();
    }

    UnloadFont(font);

    CloseWindow();

    return 0;
}
