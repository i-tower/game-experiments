#include "raylib.h" 




int main(void) {
    const int window_width = 800; 
    const int window_height = 450;

    InitWindow(window_width, window_height, "Moving Circles");

    int current_fps = 60;

    Vector2 delta_circle = {0, (float) window_height/3.0f};
    Vector2 frame_circle = {0, (float) window_height*(2.0f/3.0f)};

    const float speed = 10.0f;
    const float circle_radius = 32.0f;

    SetTargetFPS(current_fps);

    while(!WindowShouldClose()) {
        float mouse_wheel = GetMouseWheelMove();

        if (mouse_wheel != 0) {
            current_fps += mouse_wheel;
            if (current_fps < 0) current_fps = 0;
            SetTargetFPS(current_fps);
        }


        delta_circle.x += GetFrameTime()*6.0f*speed;

        frame_circle.x += 0.1f*speed;

        if(delta_circle.x > window_width) delta_circle.x  = 0;
        if(frame_circle.x > window_width) frame_circle.x = 0;

        if (IsKeyPressed(KEY_R)) {
            delta_circle.x = 0;
            frame_circle.x = 0;
           // current_fps = 60;
        }

        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawCircleV(delta_circle, circle_radius, RED);
            DrawCircleV(frame_circle, circle_radius, BLUE);

            const char* fps_text = 0;
            if (current_fps <= 0) fps_text = TextFormat("FPS: unlimited (%i)", GetFPS());
            else fps_text = TextFormat("FPS: (%i) (target: %i)", GetFPS(), current_fps);
            DrawText(fps_text, 10, 10, 20, DARKGRAY);
            DrawText(TextFormat("Frame Time: %02.02f ms", GetFrameTime()), 10, 30, 20, DARKGRAY);
            

            EndDrawing();
    }

    CloseWindow();

    return 0;

}