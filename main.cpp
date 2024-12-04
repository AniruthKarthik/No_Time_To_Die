#include "raylib.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iostream>

enum GameState { INSTRUCTIONS, GAME };
GameState currentState = INSTRUCTIONS;

class Ball {
public:
    int x, y;
    int xspeed, yspeed;
    int radius;
    Color color;
    bool destroyable; // Flag to check if the ball can be destroyed

    Ball(int maxX, int maxY, int r, Color col, int offsetX = 0, int offsetY = 0) {
        radius = r;
        x = radius + rand() % (maxX - 2 * radius - offsetX);  // Adjust for offset
        y = radius + rand() % (maxY - 2 * radius - offsetY);  // Adjust for offset
        xspeed = (rand() % 2 == 0 ? 1 : -1) * (5 + rand() % 10);
        yspeed = (rand() % 2 == 0 ? 1 : -1) * (5 + rand() % 10);
        color = col;
        destroyable = false; // Balls are not destroyable by default
    }

    void draw() const {
        DrawCircle(x, y, radius, color);
    }

    void updatePos(int screenWidth, int screenHeight) {
        x += xspeed;
        y += yspeed;

        if (x - radius <= 0) {
            x = radius;
            xspeed = abs(xspeed);
        } else if (x + radius >= screenWidth) {
            x = screenWidth - radius;
            xspeed = -abs(xspeed);
        }

        if (y - radius <= 0) {
            y = radius;
            yspeed = abs(yspeed);
        } else if (y + radius >= screenHeight) {
            y = screenHeight - radius;
            yspeed = -abs(yspeed);
        }
    }

    bool isClicked(Vector2 mousePoint) const {
        return CheckCollisionPointCircle(mousePoint, {static_cast<float>(x), static_cast<float>(y)}, radius);
    }

    void adjustSpeed(int speed) {
        xspeed = (xspeed > 0 ? 1 : -1) * speed;
        yspeed = (yspeed > 0 ? 1 : -1) * speed;
    }

    void setDestroyable(bool flag) {
        destroyable = flag;
    }
};

class Hero {
public:
    int points;
    float centerX, centerY, xvelocity, yvelocity;
    Rectangle heroRect;
    bool isMoving;
    bool isFacingRight;
    Texture2D spriteSheet;
    int currentFrame;
    int framesCounter;
    int framesSpeed;

    Hero(int p, float cX, float cY)
    : points(p), centerX(cX), centerY(cY), 
      xvelocity(0), yvelocity(0), isMoving(false), 
      isFacingRight(true),
      currentFrame(0), framesCounter(0), framesSpeed(5) {
        spriteSheet = LoadTexture("assets/scarfy.png");
        int frameWidth = spriteSheet.width / 6;
        int frameHeight = spriteSheet.height;
        
        heroRect = {
            cX - frameWidth / 2.0f, 
            cY - frameHeight / 2.0f, 
            static_cast<float>(frameWidth),   
            static_cast<float>(frameHeight)   
        };
    }

    ~Hero() {
        UnloadTexture(spriteSheet);
    }

    void draw() const {
    int frameWidth = spriteSheet.width / 6;  // Assuming 6 frames in the sprite sheet
    Rectangle sourceRec = { 
        static_cast<float>(currentFrame * frameWidth), 0.0f, 
        static_cast<float>(frameWidth), static_cast<float>(spriteSheet.height) 
    };

    // Mirror the texture when facing left
    if (!isFacingRight) {
        sourceRec.width = -sourceRec.width;
    }

    // Use heroRect for destination rectangle
    Rectangle destRec = heroRect;

    // The origin is adjusted to the top-left since heroRect already positions correctly
    Vector2 origin = { 0.0f, 0.0f };
    // Draw the texture inside the rectangle
    DrawTexturePro(spriteSheet, sourceRec, destRec, origin, 0.0f, WHITE);

    // Optional: Draw the red bounding rectangle for debugging
    // DrawRectangleLinesEx(heroRect, 2, RED);
}

    void updatePos() {
        isMoving = false;

        if (IsKeyDown(KEY_W) && heroRect.y > 0) {
            centerY -= yvelocity;
            heroRect.y -= yvelocity;
            isMoving = true;
        }
        if (IsKeyDown(KEY_S) && heroRect.y + heroRect.height < GetScreenHeight()) {
            centerY += yvelocity;
            heroRect.y += yvelocity;
            isMoving = true;
        }
        if (IsKeyDown(KEY_A) && heroRect.x > 0) {
            centerX -= xvelocity;
            heroRect.x -= xvelocity;
            isMoving = true;
            isFacingRight = false;
        }
        if (IsKeyDown(KEY_D) && heroRect.x + heroRect.width < GetScreenWidth()) {
            centerX += xvelocity;
            heroRect.x += xvelocity;
            isMoving = true;
            isFacingRight = true;
        }

        // Update animation frame only when moving
        if (isMoving) {
            updateAnimation();
        } else {
            currentFrame = 0;  // Reset to first frame when not moving
        }
    }

    void updateAnimation() {
        framesCounter++;
        if (framesCounter >= (60 / framesSpeed)) {
            framesCounter = 0;
            
            // Modify animation logic for reversed running when facing left
            if (isFacingRight) {
                currentFrame++;
                if (currentFrame >= 6) {  // Assuming 6 frames in the sprite sheet
                    currentFrame = 0;
                }
            } else {
                currentFrame--;
                if (currentFrame < 0) {  // Reverse animation when facing left
                    currentFrame = 5;
                }
            }
        }
    }

    bool checkCollision(const std::vector<Ball>& balls) {
        for (const auto& ball : balls) {
            if (CheckCollisionCircleRec({static_cast<float>(ball.x), static_cast<float>(ball.y)}, 
                                        ball.radius, heroRect)) {
                return true;  // Collision detected
            }
        }
        return false;
    }
};

void ShowInstructions(int screenWidth, int screenHeight) {
    DrawText("WELCOME TO >> NO TIME TO DIE!", static_cast<float>(screenWidth / 2 - 350), static_cast<float>(screenHeight / 4), 40, WHITE);
    DrawText("Use W, A, S, D to move the hero.", static_cast<float>(screenWidth / 2 - 250), static_cast<float>(screenHeight / 4 + 100), 30, YELLOW);
    DrawText("Avoid balls and try to survive!", static_cast<float>(screenWidth / 2 - 250), static_cast<float>(screenHeight / 4 + 150), 30, YELLOW);

    // Calculate the width of the text and center it
    const char* instructionText = "Yellow balls can be clicked to destroy.";
    int textWidth = MeasureText(instructionText, 30);
    DrawText(instructionText, static_cast<float>(screenWidth / 2 - textWidth / 2), static_cast<float>(screenHeight / 4 + 200), 30, YELLOW);

    DrawText("Press ENTER to start!", static_cast<float>(screenWidth / 2 - 200), static_cast<float>(screenHeight / 4 + 300), 30, WHITE);
}


int main() {
    int score = 0;
    srand(time(0));
    const int screenWidth = 1920;
    const int screenHeight = 1080;

    InitWindow(screenWidth, screenHeight, "No time to die");
    InitAudioDevice();
    SetTargetFPS(120);

    Texture2D background=LoadTexture("assets/nbg.png");
    Sound sound = LoadSound("assets/mdmp3.mp3");
    PlaySound(sound);

    Hero hero(5, screenWidth / 2, screenHeight / 2);
    hero.xvelocity = 5;
    hero.yvelocity = 5;

    std::vector<Ball> balls;
    std::vector<Ball> cornerBalls;
    double lastTime = GetTime();
    bool isGameOver = false;

    cornerBalls.push_back(Ball(screenWidth, screenHeight, 20, WHITE, 100, 100)); // Top-left
    cornerBalls.push_back(Ball(screenWidth, screenHeight, 20, WHITE, 100, 100)); // Bottom-right

    Rectangle retryButton = {screenWidth / 2 - 100, screenHeight / 2 + 50, 200, 50};

    double stateTime = 0.0;  // Initialize to 0, it will be set when the game starts
    bool isYellow = false;
    bool canDelete = false;

    while (!WindowShouldClose() && currentState == INSTRUCTIONS) {
        BeginDrawing();
        ClearBackground(BLACK);

        ShowInstructions(screenWidth, screenHeight);

        if (IsKeyPressed(KEY_ENTER)) {
            currentState = GAME;
            stateTime = GetTime();  // Start the game timer after ENTER is pressed
        }

        EndDrawing();
    }

    while (!WindowShouldClose()) {
        if (!isGameOver) {
            hero.updatePos();
            int newSpeed = (hero.isMoving) ? 8 : 1;

            for (auto& ball : balls) {
                ball.adjustSpeed(newSpeed);
            }
            for (auto& cornerBall : cornerBalls) {
                cornerBall.adjustSpeed(newSpeed);
            }

            double currentTime = GetTime();
            if (currentTime - lastTime >= 3.0 && !isYellow) {
                balls.push_back(Ball(screenWidth, screenHeight, 20, WHITE));
                lastTime = currentTime;
            }

            if (hero.isMoving) {
                score++;
                SetSoundPitch(sound, 1.0f);
            } else {
                SetSoundPitch(sound, 0.8f);
            }

            for (auto& ball : balls) {
                ball.updatePos(screenWidth, screenHeight);
            }
            for (auto& cornerBall : cornerBalls) {
                cornerBall.updatePos(screenWidth, screenHeight);
            }

            if (hero.checkCollision(balls) || hero.checkCollision(cornerBalls)) {
                isGameOver = true;
            }
        }

        if (GetTime() - stateTime >= 10.0 && !isYellow) {
            isYellow = true;
            canDelete = true;
            for (auto& ball : balls) {
                ball.color = YELLOW;
                ball.setDestroyable(true);
            }
            for (auto& cornerBall : cornerBalls) {
                cornerBall.color = YELLOW;
                cornerBall.setDestroyable(true);
            }
            stateTime = GetTime();
        }

        if (GetTime() - stateTime >= 3.0 && isYellow) {
            for (auto& ball : balls) {
                ball.color = WHITE;
                ball.setDestroyable(false);
            }
            for (auto& cornerBall : cornerBalls) {
                cornerBall.color = WHITE;
                cornerBall.setDestroyable(false);
            }
            isYellow = false;
            canDelete = false;
            stateTime = GetTime();
            balls.push_back(Ball(screenWidth, screenHeight, 20, WHITE));
        }

        if (canDelete) {
            Vector2 mousePoint = GetMousePosition();
            for (auto it = balls.begin(); it != balls.end();) {
                if (it->isClicked(mousePoint) && it->destroyable && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    it = balls.erase(it);
                    score += 100;
                } else {
                    ++it;
                }
            }
            for (auto it = cornerBalls.begin(); it != cornerBalls.end();) {
                if (it->isClicked(mousePoint) && it->destroyable && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    it = cornerBalls.erase(it);
                    score += 5000;
                } else {
                    ++it;
                }
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(background, 0, 0, WHITE);

        if (isGameOver) {
    // GAME OVER text
    const char* gameOverText = "GAME OVER!";
    int gameOverWidth = MeasureText(gameOverText, 50);
    DrawText(gameOverText, screenWidth / 2 - gameOverWidth / 2, screenHeight / 2 - 100, 50, RED);

    // Score text
    const char* scoreText = TextFormat("Score: %i", score);
    int scoreWidth = MeasureText(scoreText, 30);
    DrawText(scoreText, screenWidth / 2 - scoreWidth / 2, screenHeight / 2, 30, WHITE);

    // Retry button and text
    if (CheckCollisionPointRec(GetMousePosition(), retryButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        // Reset the game state
        isGameOver = false;
        score = 0;

        // Clear existing balls and recreate corner balls
        balls.clear();
        cornerBalls.clear();
        cornerBalls.push_back(Ball(screenWidth, screenHeight, 20, WHITE, 100, 100)); // Top-left
        cornerBalls.push_back(Ball(screenWidth, screenHeight, 20, WHITE, 100, 100)); // Bottom-right

        // Reset hero position and states
        hero.centerX = screenWidth / 2;
        hero.centerY = screenHeight / 2;
        hero.heroRect.x = screenWidth / 2 - hero.heroRect.width / 2;
        hero.heroRect.y = screenHeight / 2 - hero.heroRect.height / 2;
        stateTime = GetTime();
        isYellow = false;
        canDelete = false;
    }

    // Retry button rectangle
    DrawRectangleRec(retryButton, DARKGREEN);

    // Retry text
    const char* retryText = "Retry";
    int retryWidth = MeasureText(retryText, 30);
    DrawText(retryText, screenWidth / 2 - retryWidth / 2, screenHeight / 2 + 60, 30, WHITE);

        } else {
            for (const auto& ball : balls) {
                ball.draw();
            }
            for (const auto& cornerBall : cornerBalls) {
                cornerBall.draw();
            }
            hero.draw();

            DrawText(TextFormat("Score: %i", score), 20, 20, 30, WHITE);
            DrawText(TextFormat("Time: %.2f", GetTime() - stateTime), 20, 60, 30, WHITE);
        }

        EndDrawing();
    }

    UnloadSound(sound);
    UnloadTexture(background);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
