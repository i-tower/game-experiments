#include "raylib.h"


int main(void) {

    const int window_width = 800;
    const int window_height = 600;

    InitWindow(window_width, window_height, "Hello World!");

    SetTargetFPS(60);

    while(!WindowShouldClose()) {

        BeginDrawing();
        
            ClearBackground(RAYWHITE);
            DrawText("Some words in the middle", 400, 200, 18, BLACK);

        EndDrawing();

    }

    CloseWindow();

    return 0;
}
