#define DEFAULT_X 640
#define DEFAULT_Y 480
#define TARGET_FPS 60
#define FAKE_FLOAT_P 1000
#define p1_UP KEY_Q
#define p1_DOWN KEY_A
#define p2_UP KEY_KP_9
#define p2_DOWN KEY_KP_6
#include "raylib.h"
#include <time.h>
#include <stdlib.h>
#include <math.h>

struct Ball{
    float x;
    float y;
    float dx;
    float dy;
    float w;
    float h;
    Color c;
    float speed;
    int paused;
};

struct Paddle {
    float x;
    float y;
    float w;
    float h;
    Color c;
    int score;
};

float RandomRange(float Min, float Max);
void DrawBall(struct Ball ball, Vector2 ScreenSize);
void TickBall(struct Ball* ball, struct Paddle* p1, struct Paddle* p2);
void DrawPaddle(struct Paddle paddle, Vector2 ScreenSize);
void TickPaddles(struct Paddle* p1, struct Paddle* p2);
void AddScoreAndReset(struct Ball* ball, struct Paddle* player);
void FlipPaddleAddRandomMotionVector(struct Ball* ball, float minx, float maxx);
void DrawScore(int score, float x, float y, float w, float h, Vector2 ScreenSize, Color color);

int main()
{
    srand((unsigned int)time(NULL));
    SetTargetFPS(TARGET_FPS);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(DEFAULT_X, DEFAULT_Y, "PONG");
    Color BGCOLOR = BLACK;
    Color FGCOLOR = WHITE;
    Vector2 ScreenSize = { .x = (float)GetScreenWidth(), .y = (float)GetScreenHeight() };

    float ball_startx = 0.5f;
    float ball_starty = 0.5f;
    float ball_width = 0.05f;
    float ball_height = 0.05f;

    struct Ball ball = { .x = ball_startx - ball_width/2, .y = ball_starty - ball_width/2,
                         .dx = RandomRange(0.3f, 0.6f), .dy = 1 - ball.dx,
                         .w = ball_width, .h = ball_height, .c = FGCOLOR, .speed=0.01f, .paused=1};
    struct Paddle p1 = {.x=0.05f, .y=0.05f, .w = 0.025f, .h = 0.20f, .c=FGCOLOR, .score=0};
    struct Paddle p2 = {.x=1-p1.x-p1.w, p1.y, p1.w, p1.h, .c = FGCOLOR, .score=0};

    while (!WindowShouldClose()) {
        int nx = (int)GetScreenWidth();
        int ny = (int)GetScreenHeight();
        if (ScreenSize.x != nx || ScreenSize.y != ny) {
            ScreenSize.x = (float)nx;
            ScreenSize.y = (float)ny;
        }
        BeginDrawing();
        //printf("%f", ball.x);
        //printf("%f", ball.y);
        ClearBackground(BGCOLOR);
        DrawBall(ball, ScreenSize);
        DrawPaddle(p1, ScreenSize);
        DrawPaddle(p2, ScreenSize);
        DrawScore(p1.score, .2f, .2f, .1f, .2f, ScreenSize, FGCOLOR);
        DrawScore(p2.score, .7f, .2f, .1f, .2f, ScreenSize, FGCOLOR);
        if (ball.paused){
            if (IsKeyPressed(KEY_SPACE)) {
                ball.paused = 0;
            }
        } else {
            TickBall(&ball, &p1, &p2);
        }
        TickPaddles(&p1, &p2);
        EndDrawing();

    }
    CloseWindow();
}

float RandomRange(float Min, float Max){
    float n = (float)(rand() % (int)(Min * FAKE_FLOAT_P, Max * FAKE_FLOAT_P + 1)) + (Min*FAKE_FLOAT_P);
    return (n / FAKE_FLOAT_P);
}

void DrawBall(struct Ball ball, Vector2 ScreenSize) {
    int ix = (int)(ScreenSize.x * ball.x);
    int iy = (int)(ScreenSize.y * ball.y);
    int iw = (int)(ScreenSize.x * ball.w);
    int ih = (int)(ScreenSize.y * ball.h);
    DrawRectangle(ix, iy, iw, ih, ball.c);
}

void TickBall(struct Ball* ball, struct Paddle* p1, struct Paddle* p2) {
    ball->x += ball->dx*ball->speed;
    ball->y += ball->dy*ball->speed;
    if (ball->y < 0) {
        ball->y = fabsf(ball->y);
        ball->dy *= -1;
    } else if (ball->y + ball->h > 1) {
        ball->y += 1 - (ball->y + ball->h);
        ball->dy *= -1;
    }

    if (ball->x + ball->w > p2->x) {
        if (ball->y < p2->y + p2->h && ball->y + ball->h > p2->y) {
            ball->x += p2->x - (ball->x + ball->w);
            //ball->dx *= -1;
            FlipPaddleAddRandomMotionVector(ball, 0.3f, 0.6f);
        } else {
            AddScoreAndReset(ball, p1);
        }
    } else if (ball->x < p1->x + p1->w) {
        if (ball->y < p1->y + p1->h && ball->y + ball->h > p1->y) {
            ball->x += (p1->x + p1->w) - ball->x;
            //ball->dx *= -1;
            FlipPaddleAddRandomMotionVector(ball, 0.3f, 0.6f);
        } else {
            AddScoreAndReset(ball, p2);
        }
    }
}

void AddScoreAndReset(struct Ball* ball, struct Paddle* player) {
    player->score++;
    ball->x = 0.5f - ball->w / 2;
    ball->y = 0.5f - ball->h / 2;
    ball->speed = 0.01f;
    ball->paused = 1;
}

void FlipPaddleAddRandomMotionVector(struct Ball* ball, float minx, float maxx) {
    int flip_to_negative = 1;
    if (ball->dx < 0) {
        flip_to_negative = 0;
    }
    ball->dx = RandomRange(minx, maxx);
    while (ball->dx < minx || ball->dx > maxx) {
        ball->dx = RandomRange(minx, maxx);
    }

    if (flip_to_negative) {
        ball->dx *= -1;
    }

    if (ball->dy < 0){
        ball->dy = 1.0f - fabsf(ball->dx);
        ball->dy *= -1;
    }
    else {
        ball->dy = 1.0f - fabsf(ball->dx);
    }
    ball->speed = (RandomRange(0.006f, 0.014f) + RandomRange(0.008f, 0.012f)) / 2;
}

void DrawPaddle(struct Paddle paddle, Vector2 ScreenSize) {
    int ix = (int)(ScreenSize.x * paddle.x);
    int iy = (int)(ScreenSize.y * paddle.y);
    int iw = (int)(ScreenSize.x * paddle.w);
    int ih = (int)(ScreenSize.y * paddle.h);
    DrawRectangle(ix, iy, iw, ih, paddle.c);
}

void TickPaddles(struct Paddle* p1, struct Paddle* p2) {
    if (IsKeyDown(p1_UP)) {
        if(p1->y > 0){
            p1->y -= 0.01f;
        }
    }
    if (IsKeyDown(p1_DOWN)) {
        if(p1->y + p1->h < 1){
            p1->y += 0.01f;
        }
    }
    if (IsKeyDown(p2_UP)) {
        if(p2->y > 0){
            p2->y -= 0.01f;
        }
    }
    if (IsKeyDown(p2_DOWN)) {
        if(p2->y + p2->h < 1){
            p2->y += 0.01f;
        }
    }
}

void DrawScore(int score, float x, float y, float width, float height, Vector2 ScreenSize, Color color) {
    float x0 = x * ScreenSize.x;
    float y0 = y * ScreenSize.y;
    float x1 = (x + width) * ScreenSize.x;
    float y1 = (y + height) * ScreenSize.y;
    float strokeWidth = 0.01f * ScreenSize.x;
    float w = width * ScreenSize.x;
    float h = height * ScreenSize.y;

    int arrayLookup[10] = {7,3,5,9,4,7,5,6,8,5};
    Vector2 _0[7] = {
        {x0, y1},
        {x0, y0},
        {x1, y0},
        {x0, y1},
        {x1, y1},
        {x1, y0},
        {x0, y1} };
    Vector2 _1[3] = {
        {x0 + w * .2f, y1},
        {x0 + w * .2f, y0},
        {x0, y0 + h * .1f} };

    Vector2 _2[5] ={
        { x0, y0 + h * .25f },
        { x0 + w*.5f, y0 },
        { x1, y0+h*.25f },
        { x0, y1 },
        { x1,y1 }};

    Vector2 _3[9] = {
        { x0, y0 + h * .25f },
        { x0+ w*.5f, y0 },
        { x1, y0 + h * .25f },
        { x0 + w*.5f, y0+h*.5f },
        { x0, y0+h*.5f},
        { x0 +w* .5f, y0+h*.5f },
        { x1, y0 + h * .75f },
        { x0 +w * .5f, y1 },
        { x0,y1 }};

    Vector2 _4[4] = {
        {x0+w*.75f,y1},
        {x0 + w * .75f,y0},
        {x0, y0+h*.5f},
        {x1, y0+h*.5f},
    };

    Vector2 _5[7] = {
        {x1, y0},
        {x0, y0},
        {x0, y0 + h*.5f},
        {x0+w*.5f, y0 + h*.25f},
        {x1, y0+h*.5f},
        {x0+w*.5f, y1},
        {x0, y0+h*.75f}
    };

    Vector2 _6[5] = {
        {x0, y0},
        {x0, y1},
        {x1, y1},
        {x0 + w * .5f, y0 + h * .5f},
        {x0, y0 + h * .5f},
    };
    
    Vector2 _7[6] = {
        {x0, y0},
        {x1, y0},
        {x0, y1},
        {x0 + w * .5f, y0 + h * .5f},
        {x0 + w * .25f, y0 + h * .5f},
        {x0 + w * .75f, y0 + h * .5f},
    };

    Vector2 _8[8] = {
        {x0+w*.25f,y0+h*.5f},
        {x0 + w * .25f,y0},
        {x1, y0},
        {x1, y0 + h *.5f},
        {x0, y0 + h*.5f},
        {x0, y1},
        {x1, y1},
        {x1, y0 +h*.5f},
    };
    Vector2 _9[5] = {
        {x1, y0+h*.5f},
        {x0, y0 + h * .5f},
        {x0, y0},
        {x1, y0},
        {x1, y1}
    };
    Vector2 _w[5] = {
        {x0, y0},
        {x0+w*.25f, y1},
        {x0+w*.5f, y0},
        {x0+w*.75f, y1},
        {x1, y0}
    };

    switch (score) {
    case 0:
        DrawSplineLinear(_0, arrayLookup[0], strokeWidth, WHITE);
        break;
    case 1:
        DrawSplineLinear(_1, arrayLookup[1], strokeWidth, WHITE);
        break;
    case 2:
        DrawSplineLinear(_2, arrayLookup[2], strokeWidth, WHITE);
        break;
    case 3:
        DrawSplineLinear(_3, arrayLookup[3], strokeWidth, WHITE);
        break;
    case 4:
        DrawSplineLinear(_4, arrayLookup[4], strokeWidth, WHITE);
        break;
    case 5:
        DrawSplineLinear(_5, arrayLookup[5], strokeWidth, WHITE);
        break;
    case 6:
        DrawSplineLinear(_6, arrayLookup[6], strokeWidth, WHITE);
        break;
    case 7:
        DrawSplineLinear(_7, arrayLookup[7], strokeWidth, WHITE);
        break;
    case 8:
        DrawSplineLinear(_8, arrayLookup[8], strokeWidth, WHITE);
        break;
    case 9:
        DrawSplineLinear(_9, arrayLookup[9], strokeWidth, WHITE);
        break;
    default:
        DrawSplineLinear(_w, 5, strokeWidth, WHITE);
        break;
    }

    

}