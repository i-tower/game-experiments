#include "raylib.h"


#define TEAM_RED 1
#define TEAM_BLACK 0
#define DEFAULT_PADDLE_SIZE {20, 80}
#define DEFAULT_BALL_SIZE {20, 20}
#define GAME_SPEED 6

#define UNUSED(x) (void)(x)

typedef struct Player {
    Vector2 position;
    Vector2 prev_position;
    Vector2 paddle_size;
    Color color;
    int team;
    int speed;
    int score;
} Player;

typedef struct Ball {
    Vector2 position;
    Vector2 speed;
    Vector2 size;
    Color color;
    
} Ball;

typedef enum GameState {
    PLAYING,
    GAME_START,
    GAME_OVER,
} GameState;

typedef struct GameContext {
    Vector2 window_size;
    Player players[2];
    Ball ball;
    GameState state;
    int game_speed;
    int num_players;
    int round;
    int max_score;
    Player* last_scored;
    
} GameContext;

void InitPlayer(GameContext* context, int team);
void InitBall(GameContext* context);
void UpdateScene(GameContext* context);
void GameStart(GameContext* context);
void GameOver(GameContext* context);
void HandleCollision(GameContext* context);
void ReflectBall(Ball* ball, Player* player);
void Score(GameContext* context, int team);
void DrawGame(GameContext* context);
void DrawGameOver(GameContext* context);
void ResetBall(GameContext* context);
void Serve(GameContext* context);


// FIXME: I don't like passing the game context around and diving deep through multiple
// references every time i have to write some function to update the ball position or the
// like. Use variables inside each of the functions recieving the context to help with the
// typing? 

// FIXME: Handle framerates -> GetFrameTime();

int main (void) {


    const int window_width = 1200;
    const int window_height = 800;

    InitWindow(window_width, window_height, "Pong");

    GameContext Context = {
        .window_size = {window_width, window_height},
        .game_speed = GAME_SPEED,
        .num_players = 2,
        .round = 0,
        .state = GAME_START,
        .max_score = 3
    };
    
    
    InitPlayer(&Context, TEAM_BLACK); // Player 1
    InitPlayer(&Context, TEAM_RED);   // Player 2
    
    InitBall(&Context);


    SetTargetFPS(30);

    while(!WindowShouldClose()) {

        ClearBackground(RAYWHITE);

        UpdateScene(&Context);

        if (Context.state == GAME_OVER) {
            DrawGameOver(&Context);
        } else {
            DrawGame(&Context);
        }

    }

    CloseWindow();

    return 0;
}

void InitPlayer(GameContext* context, int team) {
    
    Player player = {
        .color = (team) ? RED : BLACK,
        .paddle_size = DEFAULT_PADDLE_SIZE,
        .position = {(team) ? context->window_size.x*0.9f : 
                              context->window_size.x*0.1f, 0.0f},
        .score = 0,
        .team = team
    };

    player.position.y = context->window_size.y/2.0f - player.paddle_size.y / 2.0f;

    if (team == 0) context->players[0] = player;
    else context->players[1] = player;
}


void InitBall(GameContext* context) {

    Ball ball = {
        .color = BLACK,
        .size = DEFAULT_BALL_SIZE,
        .speed = {0.0f, 0.0f}
    };

    ball.position.x = context->window_size.x/2.0f - ball.size.x/2.0f;
    ball.position.y = context->window_size.y/2.0f - ball.size.y/2.0f;

    context->ball = ball;

}

void ResetBall(GameContext* context) {

    if (context->round >= 0) {
        context->ball.position.x = context->window_size.x/2.0f - context->ball.size.x/2.0f;
        context->ball.position.y = context->window_size.y/2.0f - context->ball.size.y/2.0f;
        context->ball.speed.x = 0;
        context->ball.speed.y = 0;
    }

}


void GameStart(GameContext* context) {

    ResetBall(context);
    // INPROGRESS: Give ball a random direction on launch

    if (context->round > 0) {
        Serve(context);
        return; // gross
    }

    if(IsKeyPressed(KEY_SPACE)) {
        if (GetRandomValue(0, 1)) {
            context->ball.speed.x = 2;
        } else {
            context->ball.speed.x = -2;
        }
        context->round++;
        context->state = PLAYING;
    } 
    
}

// TODO: Add serving game state
void Serve(GameContext* context) {
    Ball* ball = &context->ball;
    Player* player = context->last_scored;
    
    if (player->team == TEAM_BLACK) {
        ball->position.x = player->position.x + player->paddle_size.x + 20;
    } else {
        ball->position.x = player->position.x - player->paddle_size.x - 20;
    }
    ball->position.y = player->position.y + (player->paddle_size.y / 2);
    
    if (IsKeyPressed(KEY_SPACE)) {
        if (player->team == TEAM_BLACK) {
            ball->speed.x = 2;
        } else {
            ball->speed.x = -2;
        }
        context->state = PLAYING;
    }
}

void GameOver(GameContext* context) {

    ResetBall(context);

    if (IsKeyPressed(KEY_SPACE)) {
        context->round = 0;
        context->players[0].score = 0;
        context->players[1].score = 0;
        context->state = GAME_START;
    }

}

void UpdateScene(GameContext* context) { 
    
    if (context->state == GAME_START) {
        GameStart(context);
    } else if (context->state == PLAYING) { 
        context->ball.position.x += context->ball.speed.x * context->game_speed;
        context->ball.position.y += context->ball.speed.y * context->game_speed;
    } else if (context->state == GAME_OVER) {
        GameOver(context);
    }
    
    // TODO: Move screen edge detection to collision function? Also could factor out
    //       paddle movement to a paddle speed variable.

    context->players[0].prev_position = context->players[0].position;
    context->players[1].prev_position = context->players[1].position;

    if ((IsKeyDown(KEY_W)) && context->players[0].position.y >= 0) {
        context->players[0].position.y -= 1 * context->game_speed; // paddle movement
        if (context->players[0].position.y < 0) context->players[0].position.y = 0;
    } else if(IsKeyDown(KEY_S)) {
        context->players[0].position.y += 1 * context->game_speed; // paddle movement
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
    
    
    HandleCollision(context);

}

void Score(GameContext* context, int team) {

    const int max_score = context->max_score;

    if (!team) {
        context->players[0].score++;
        context->last_scored = &context->players[0];
    } else {
        context->players[1].score++;
        context->last_scored = &context->players[1];
    }

    context->state = GAME_START;

    if (context->players[0].score >= max_score || context->players[1].score >= max_score) {
        context->state = GAME_OVER; 
    }
}


void HandleCollision(GameContext* context) {


    // Handle screen edge collisions
    if (context->ball.position.x >= context->window_size.x - context->ball.size.x) {
        // Player 1 Scores
        Score(context, TEAM_BLACK);
    } else if (context->ball.position.x <= 0) {
        // Player 2 Scores
        Score(context, TEAM_RED);
    }
    if (context->ball.position.y >= context->window_size.y - context->ball.size.y || context->ball.position.y <= 0) {
        context->ball.speed.y *= -1;
    }


    
    
    /* FIXME: Possible for the ball become stuck inside the paddle. 
       Need to refine collision detection.
       Reposition the ball to the edge of the paddle at collision? */
    
    // Left player detection.
    
    /*  Paddle collision detection:
        Is ball left edge left of paddle right edge
        Is ball bottom below paddle top
        Is ball top above paddle bottom */
    if (context->ball.position.x <= context->players[0].position.x + context->players[0].paddle_size.x &&
        context->ball.position.y + context->ball.size.y >= context->players[0].position.y && 
        context->ball.position.y <= context->players[0].position.y + context->players[0].paddle_size.y &&
        context->ball.position.x + context->ball.size.x >= context->players[0].position.x) {

            ReflectBall(&context->ball, &context->players[0]);

    }

    
    // Right player detection
    if (context->ball.position.x + context->ball.size.x >= context->players[1].position.x &&
        context->ball.position.y + context->ball.size.y >= context->players[1].position.y && 
        context->ball.position.y <= context->players[1].position.y + context->players[1].paddle_size.y &&
        context->ball.position.x <= context->players[1].position.x + context->players[1].paddle_size.x){

            ReflectBall(&context->ball, &context->players[1]);
            
        }

}


// TODO: Add ball spin 
void ReflectBall(Ball* ball, Player* player) {
    float paddle_speed = player->position.y - player->prev_position.y;
    float max_spin = 1.5;
    float min_spin = -1.5;
    float fuzz = GetRandomValue(95, 105) / 100.0f;

    ball->speed.x *= -1;
    ball->speed.y += paddle_speed * 0.1 * fuzz;
    
    if (ball->speed.y > max_spin) ball->speed.y = max_spin;
    if (ball->speed.y < min_spin) ball->speed.y = min_spin;
}


void DrawGame(GameContext* Context) {
    BeginDrawing();

        DrawRectangleV(Context->players[0].position, Context->players[0].paddle_size, Context->players[0].color);
        DrawRectangleV(Context->players[1].position, Context->players[1].paddle_size, Context->players[1].color);
        DrawRectangleV(Context->ball.position, Context->ball.size, Context->ball.color);

        if (Context->state == GAME_START) {
            DrawText("Press space to start", 400, 200, 32, BLACK);
        }

        DrawText(TextFormat("Player 1 Score: %i", Context->players[0].score), 50, 30, 32, BLACK);
        DrawText(TextFormat("Player 2 Score: %i", Context->players[1].score), 850, 30, 32, BLACK);

    EndDrawing();
}
 
void DrawGameOver(GameContext* context) {
    BeginDrawing();

        DrawText("Game Over!", 400, 300, 50, BLACK);
        DrawText(TextFormat("Player %i wins", (context->players[0].score > context->players[1].score) ? 1 : 2), 400, 400, 40, BLACK);

    EndDrawing();
}