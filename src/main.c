#include "raylib.h"


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
    int team;
    int speed;
    int score;
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
    int max_score;
    int in_progress;
    int game_over;
    
} GameContext;

void InitPlayer(GameContext* context, int team);
void InitBall(GameContext* context);
void UpdateScene(GameContext* context);
void GameStart(GameContext* context);
void HandleCollision(GameContext* context);
void ReflectBall(Ball* ball, Player* player);
void Score(GameContext* context, int team);
void DrawGame(GameContext* context);
void DrawGameOver(GameContext* context);


// FIXME: I don't like passing the game context around and diving deep through multiple
// references every time i have to write some function to update the ball position or the
// like. Use variables inside each of the functions recieving the context to help with the
// typing? 

// FIXME: Handle framerates -> GetFrameTime();
// FIXME: Game resets incorrectly. After game over pressing space should return
//        initial game conditions. Currently pressing space at the game over screen
//        immediately begins a new game. <<< This seems to be working fine at the moment?

int main (void) {


    const int window_width = 1200;
    const int window_height = 800;

    InitWindow(window_width, window_height, "Pong");

    GameContext Context = {
        .window_size = {window_width, window_height},
        .game_speed = GAME_SPEED,
        .num_players = 2,
        .round = 1,
        .in_progress = 0,
        .game_over = 0,
    };
    
    
    InitPlayer(&Context, TEAM_BLACK); // Player 1
    InitPlayer(&Context, TEAM_RED);   // Player 2
    
    InitBall(&Context);


    SetTargetFPS(30);

    while(!WindowShouldClose()) {

        ClearBackground(RAYWHITE);

        UpdateScene(&Context);

        if (Context.game_over) {
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
        .velocity = {0.0f, 0.0f}
    };

    ball.position.x = context->window_size.x/2.0f - ball.size.x/2.0f;
    ball.position.y = context->window_size.y/2.0f - ball.size.y/2.0f;

    context->ball = ball;

}

// FIXME: Game reset -> See note above main
void GameStart(GameContext* context) {

    if (IsKeyPressed(KEY_SPACE) && context->game_over) {
        context->round = 0;
        context->game_over = 0;
        context->players[0].score = 0;
        context->players[1].score = 0;
    }

    if (context->round > 0 && context->in_progress == 0) {
        context->ball.position.x = context->window_size.x/2.0f - context->ball.size.x/2.0f;
        context->ball.position.y = context->window_size.y/2.0f - context->ball.size.y/2.0f;
        context->ball.velocity.x = 0;
        context->ball.velocity.y = 0;
    }



    if(IsKeyPressed(KEY_SPACE)) {
        context->ball.velocity.x = 2;
        context->in_progress = 1;
        context->round++;
    } 
    
}


void UpdateScene(GameContext* context) { 
    
    if (!context->in_progress) {
        GameStart(context);
    } else { 
        context->ball.position.x += context->ball.velocity.x * context->game_speed;
        context->ball.position.y += context->ball.velocity.y * context->game_speed;
    }
    
    // TODO: Move screen edge detection to collision function? Also could factor out
    //       paddle movement to a paddle speed variable.
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

    if (!team) {
        context->players[0].score++;
    } else {
        context->players[1].score++;
    }

    context->in_progress = 0;

    if (context->players[0].score >= 3 || context->players[1].score >= 3) {
        context->game_over = 1; 
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
        context->ball.velocity.y *= -1;
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

            ReflectBall(&context->ball, &context->players[0]);
            
        }

}

// TODO: Add ball spin 
void ReflectBall(Ball* ball, Player* player) {

    ball->velocity.x *= -1;

    // if (IsKeyDown(KEY_W)) {
    //     ball->velocity.y += 0.3f;
    // } else if (IsKeyDown(KEY_S)) {
    //     ball->velocity.y -= 0.3f; 
    // }

}

void DrawGame(GameContext* Context) {
    BeginDrawing();

        DrawRectangleV(Context->players[0].position, Context->players[0].paddle_size, Context->players[0].color);
        DrawRectangleV(Context->players[1].position, Context->players[1].paddle_size, Context->players[1].color);
        DrawRectangleV(Context->ball.position, Context->ball.size, Context->ball.color);

        if (!Context->in_progress) {
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