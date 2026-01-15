#include "raylib.h"

#define TEAM_RED 1
#define TEAM_BLACK 0
#define DEFAULT_PADDLE_SIZE {20, 80}
#define DEFAULT_BALL_SIZE {20, 20}
#define GAME_SPEED 4

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
    Player players[2];
    Ball* ball;
    int game_speed;
    int num_players;
    int round;
    
} GameContext;

void InitPlayer(GameContext* context, int team);
Ball InitBall(GameContext* context);
void UpdateScene(GameContext* context);

int main (void) {


    const int window_width = 1200;
    const int window_height = 800;

    InitWindow(window_width, window_height, "Pong");

    GameContext Context = {
        .window_size = {window_width, window_height},
        .game_speed = GAME_SPEED,
        .num_players = 2,
        .round = 1
    };
    
    
    InitPlayer(&Context, TEAM_BLACK); // Player 1
    InitPlayer(&Context, TEAM_RED);   // Player 2
    
    Ball ball = InitBall(&Context);


    SetTargetFPS(30);

    while(!WindowShouldClose()) {

        ClearBackground(RAYWHITE);

        UpdateScene(&Context);

        BeginDrawing();

            DrawRectangleV(Context.players[0].position, Context.players[0].paddle_size, Context.players[0].color);
            DrawRectangleV(Context.players[1].position, Context.players[1].paddle_size, Context.players[1].color);
            DrawRectangleV(ball.position, ball.size, ball.color);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

void InitPlayer(GameContext* context, int team) {
    
    Player player = {
        .color = (team) ? RED : BLACK,
        .paddle_size = DEFAULT_PADDLE_SIZE,
        .position = {(team) ? context->window_size.x*0.9f : 
                              context->window_size.x*0.1f, 0.0f}
    };

    player.position.y = context->window_size.y/2.0f - player.paddle_size.y / 2.0f;

    if (team == 0) context->players[0] = player;
    else context->players[1] = player;
}


Ball InitBall(GameContext* context) {

    Ball ball = {
        .color = BLACK,
        .size = DEFAULT_BALL_SIZE,
        .velocity = {0, 0}
    };

    ball.position.x = context->window_size.x/2.0f - ball.size.x/2.0f;
    ball.position.y = context->window_size.y/2.0f - ball.size.y/2.0f;

    return ball;

}

// Only to be called in the main loop
void UpdateScene(GameContext* context) { 
    
    if (IsKeyPressed(KEY_W) || IsKeyDown(KEY_W)) {
        context->players[0].position.y -= 1 * context->game_speed;
    } else if(IsKeyPressed(KEY_S) || IsKeyDown(KEY_S)) {
        context->players[0].position.y += 1 * context->game_speed;
    }

     if (IsKeyPressed(KEY_UP) || IsKeyDown(KEY_UP)) {
        context->players[1].position.y -= 1 * context->game_speed;
    } else if(IsKeyPressed(KEY_DOWN) || IsKeyDown(KEY_DOWN)) {
        context->players[1].position.y += 1 * context->game_speed;
    }

}