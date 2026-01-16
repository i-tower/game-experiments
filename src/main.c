#include "raylib.h"
#include "raymath.h"

#define TEAM_RED 1
#define TEAM_BLACK 0
#define DEFAULT_PADDLE_SIZE {20, 80}
#define DEFAULT_BALL_SIZE {20, 20}
#define GAME_SPEED 6

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
    Ball ball;
    int game_speed;
    int num_players;
    int round;
    int in_progress;
    
} GameContext;

void InitPlayer(GameContext* context, int team);
void InitBall(GameContext* context);
void UpdateScene(GameContext* context);
void GameStart(GameContext* context);
void HandleCollision(GameContext* context);
void ReflectBall(Ball* ball);

int main (void) {


    const int window_width = 1200;
    const int window_height = 800;

    InitWindow(window_width, window_height, "Pong");

    GameContext Context = {
        .window_size = {window_width, window_height},
        .game_speed = GAME_SPEED,
        .num_players = 2,
        .round = 1,
        .in_progress = 0
    };
    
    
    InitPlayer(&Context, TEAM_BLACK); // Player 1
    InitPlayer(&Context, TEAM_RED);   // Player 2
    
    InitBall(&Context);


    SetTargetFPS(30);

    while(!WindowShouldClose()) {

        ClearBackground(RAYWHITE);

        UpdateScene(&Context);

        BeginDrawing();

            DrawRectangleV(Context.players[0].position, Context.players[0].paddle_size, Context.players[0].color);
            DrawRectangleV(Context.players[1].position, Context.players[1].paddle_size, Context.players[1].color);
            DrawRectangleV(Context.ball.position, Context.ball.size, Context.ball.color);

            if (!Context.in_progress) {
                DrawText("Press space to start", 400, 200, 32, BLACK);
            }

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


void InitBall(GameContext* context) {

    Ball ball = {
        .color = BLACK,
        .size = DEFAULT_BALL_SIZE,
        .velocity = {0.0f, 0.0f}
    };

    ball.position.x = context->window_size.x/2.0f - ball.size.x/2.0f;
    ball.position.y = context->window_size.y/2.0f - ball.size.y/2.0f;

    context->ball = ball;

}

void GameStart(GameContext* context) {

    if(IsKeyPressed(KEY_SPACE)) {
        context->ball.velocity.x = 2;
        context->in_progress = 1;
    }
}


void UpdateScene(GameContext* context) { 
    

    if ((IsKeyPressed(KEY_W) || IsKeyDown(KEY_W)) && context->players[0].position.y >= 0) {
        context->players[0].position.y -= 1 * context->game_speed;
        if (context->players[0].position.y < 0) context->players[0].position.y = 0;
    } else if(IsKeyPressed(KEY_S) || IsKeyDown(KEY_S)) {
        context->players[0].position.y += 1 * context->game_speed;
        if (context->players[0].position.y + context->players[0].paddle_size.y > context->window_size.y) {
            
            context->players[0].position.y = context->window_size.y - context->players[0].paddle_size.y;
        } 
    }
    
    if ((IsKeyPressed(KEY_UP) || IsKeyDown(KEY_UP)) && context->players[1].position.y >= 0) {
        context->players[1].position.y -= 1 * context->game_speed;
        if (context->players[1].position.y < 0) context->players[1].position.y = 0;
    } else if(IsKeyPressed(KEY_DOWN) || IsKeyDown(KEY_DOWN)) {
        context->players[1].position.y += 1 * context->game_speed;
        if (context->players[1].position.y + context->players[1].paddle_size.y > context->window_size.y) {
            
            context->players[1].position.y = context->window_size.y - context->players[1].paddle_size.y;
        } 
    }
    
    if (!context->in_progress) {
        GameStart(context);
    } else { 
        // The game is in progress update the ball position as well.
        context->ball.position.x += context->ball.velocity.x * context->game_speed;
        context->ball.position.y += context->ball.velocity.y * context->game_speed;
    }
    
    HandleCollision(context);

}


void HandleCollision(GameContext* context) {


    // Handle screen edge collisions
    if (context->ball.position.x >= context->window_size.x - context->ball.size.x || context->ball.position.x <= 0) {
        // Eventually scoring should happen here
        context->ball.velocity.x *= -1;
    }
    if (context->ball.position.y >= context->window_size.y - context->ball.size.y || context->ball.position.y <= 0) {
        context->ball.velocity.y *= -1;
    }


    /* 
        Paddle collision detection:
        Is ball left edge left of paddle right edge
        Is ball bottom below paddle top
        Is ball top above paddle bottom 
    */
    // Left player detection.
    if (context->ball.position.x <= context->players[0].position.x + context->players->paddle_size.x &&
        context->ball.position.y + context->ball.size.y >= context->players[0].position.y && 
        context->ball.position.y <= context->players[0].position.y + context->players[0].paddle_size.y) {

            ReflectBall(&context->ball);

    }

    // Right player detection

    if (context->ball.position.x + context->ball.size.x >= context->players[1].position.x &&
        context->ball.position.y + context->ball.size.y >= context->players[1].position.y && 
        context->ball.position.y <= context->players[1].position.y + context->players[1].paddle_size.y){

            ReflectBall(&context->ball);
            
        }

}

void ReflectBall(Ball* ball) {

            ball->velocity.x *= -1;
            //ball->velocity.y *= -1;

            if (IsKeyDown(KEY_W)) {
                ball->velocity.y += 0.3f;
            } else if (IsKeyDown(KEY_S)) {
                ball->velocity.y -= 0.3f; 
            }

}