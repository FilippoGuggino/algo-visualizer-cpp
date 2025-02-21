#include <raylib.h>
#include <rlgl.h>
#include <raymath.h>
#include <tweeny.h>
#include <iostream>

Color current_color = BLACK;

extern "C" {
void on_click_button()
{
    current_color = RED;
}
}

int p;

struct GravityEasing {
    float operator()(float dt, int start, int end)
    {
        return float(end - start) - 0.5 * 9.8 * dt * dt;
    }
} gravity_easing;

int main(void)
{
    InitWindow(800, 600, "WebAssembly");

    Font font = LoadFont("resources/fonts/nokiafc22.ttf");
    Texture texture = LoadTexture("resources/images/logo.png");
    Camera3D camera = { 0 };
    camera.position = (Vector3) { 30.0f, 20.0f, 30.0f }; // Camera position
    camera.target = (Vector3) { 0.0f, 0.0f, 0.0f }; // Camera looking at point
    camera.up = (Vector3) { 0.0f, 1.0f, 0.0f }; // Camera up vector (rotation towards target)
    camera.fovy = 70.0f; // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE; // Camera projection type

    Mesh mesh = GenMeshCube(5, 5, 5);
    Model cube = LoadModelFromMesh(mesh);

    SetTargetFPS(60);
    float rotation = 0;
    auto y_position_animation = tweeny::from(0.0).to(50.0).during(30).via(gravity_easing); // .to(0.0).during(50).via(tweeny::easing::quadraticIn);
    while (!WindowShouldClose()) {
        rotation += 0.1;
        BeginDrawing();

        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        // rlPushMatrix();
        // rlRotatef(rotation, 0.5, 0.5, 0);
        // DrawCube(Vector3(-5, 0, 0), 10, 10, 10, BLACK);
        // rlPopMatrix();

        // rlPushMatrix();
        // rlRotatef(rotation, 0, 0, 0.5);
        // DrawCube(Vector3(5, 0, 0), 10, 10, 10, BLACK);
        // rlPopMatrix();

        if (IsKeyDown(KEY_SPACE)) {
            y_position_animation.seek(0);
        }

        float z_position = y_position_animation.step(1);

        cube.transform = MatrixRotateXYZ((Vector3) { DEG2RAD * rotation, 0, 0 });
        DrawModel(cube, Vector3 { 10.0, 0.0, z_position }, 2.0, current_color);

        EndMode3D();
        // DrawTextEx(font, "Hello, world", Vector2 { 190, 200 }, 20, 2, LIGHTGRAY);
        // DrawTexture(texture, 300, 400, WHITE);

        EndDrawing();
    }

    UnloadFont(font);

    CloseWindow();

    return 0;
}
