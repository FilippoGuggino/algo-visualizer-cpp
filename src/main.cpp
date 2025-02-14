#include <raylib.h>

int main(void)
{
    InitWindow(800, 600, "WebAssembly");
    
    Font font = LoadFont("resources/fonts/nokiafc22.ttf");
    Texture texture = LoadTexture("resources/images/logo.png");

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawTextEx(font, "Hello, world", Vector2{190, 200}, 20, 2, LIGHTGRAY);
        DrawTexture(texture, 300, 400, WHITE);

        EndDrawing();
    }

    UnloadFont(font);

    CloseWindow();

    return 0;
}
