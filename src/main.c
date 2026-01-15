#include "raylib.h"

#define TEAM_RED 1
#define TEAM_BLACK 0
#define DEFAULT_PADDLE_SIZE {20, 80}
#define DEFAULT_BALL_SIZE {20, 20}

#define UNUSED(x) (void)(x)

typedef struct Player {
    Vector2 position;
    Vector2 paddle_size;
    Color color;
} Player;

typedef struct Ball {
    Vector2 position;
    Vector2 velocity;
    Vector2 size;
    Color color;
    
} Ball;

typedef struct GameContext {
    Vector2 window_size;
    int round;
    
} GameContext;

Player InitPlayer(const GameContext* context, int team);
Ball InitBall(const GameContext* context);

int main (void) {


    const int window_width = 1200;
    const int window_height = 800;

    InitWindow(window_width, window_height, "Pong");
    
    GameContext Context = {
        .window_size = {window_width, window_height}
    };
    
    Player player1 = InitPlayer(&Context, TEAM_BLACK);
    Player player2 = InitPlayer(&Context, TEAM_RED);
    
    Ball ball = InitBall(&Context);
    

    while(!WindowShouldClose()) {

        ClearBackground(RAYWHITE);

        BeginDrawing();

            DrawRectangleV(player1.position, player1.paddle_size, player1.color);
            DrawRectangleV(player2.position, player2.paddle_size, player2.color);
            DrawRectangleV(ball.position, ball.size, ball.color);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

Player InitPlayer(const GameContext* context, int team) {
    
    Player player = {
        .color = (team) ? RED : BLACK,
        .paddle_size = DEFAULT_PADDLE_SIZE,
        .position = {(team) ? context->window_size.x*0.9f : 
                              context->window_size.x*0.1f, 0}
    };

    player.position.y = context->window_size.y/2.0f - player.paddle_size.y / 2.0f;

    return player;
}


Ball InitBall(const GameContext* context) {

    Ball ball = {
        .color = BLACK,
        .size = DEFAULT_BALL_SIZE,
        .velocity = {0, 0}
    };

    ball.position.x = context->window_size.x/2.0f - ball.size.x/2.0f;
    ball.position.y = context->window_size.y/2.0f - ball.size.y/2.0f;

    return ball;

}