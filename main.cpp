#include "raylib.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iostream>

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
    int points, outerRadius, innerRadius;
    float centerX, centerY, xvelocity, yvelocity;
    Vector2 vertices[10];
    bool isMoving;

    Hero(int p, int oRadius, int iRadius, float cX, float cY)
        : points(p), outerRadius(oRadius), innerRadius(iRadius), centerX(cX), centerY(cY), xvelocity(0), yvelocity(0), isMoving(false) {
        calculateVertices();
    }

    void calculateVertices() {
        for (int i = 0; i < points * 2; i++) {
            float angle = (2.0f * PI / points) * i;
            float radius = (i % 2 == 0) ? outerRadius : innerRadius;
            vertices[i] = {static_cast<float>(centerX + radius * cos(angle)), 
                           static_cast<float>(centerY + radius * sin(angle))};
        }
    }

    void draw() const {
        for (int i = 0; i < points * 2; i++) {
            int nextIndex = (i + 1) % (points * 2);
            DrawLineV(vertices[i], vertices[nextIndex], WHITE);
        }
    }

    void updatePos() {
        isMoving = false;

        if (IsKeyDown(KEY_W) && centerY - outerRadius > 0) {
            centerY -= yvelocity;
            isMoving = true;
        }
        if (IsKeyDown(KEY_S) && centerY + outerRadius < GetScreenHeight()) {
            centerY += yvelocity;
            isMoving = true;
        }
        if (IsKeyDown(KEY_A) && centerX - outerRadius > 0) {
            centerX -= xvelocity;
            isMoving = true;
        }
        if (IsKeyDown(KEY_D) && centerX + outerRadius < GetScreenWidth()) {
            centerX += xvelocity;
            isMoving = true;
        }

        calculateVertices();
    }

    bool checkCollision(const std::vector<Ball>& balls) {
        for (const auto& ball : balls) {
            int dx = ball.x - centerX;
            int dy = ball.y - centerY;
            int distanceSquared = dx * dx + dy * dy;
            if (distanceSquared <= (outerRadius + ball.radius) * (outerRadius + ball.radius)) {
                return true;  // Collision detected
            }
        }
        return false;
    }
};

int main() {
    int score = 0;
    srand(time(0));
    const int screenWidth = 1920;
    const int screenHeight = 1080;

    InitWindow(screenWidth, screenHeight, "No time to die");
    SetTargetFPS(120);

    Hero hero(5, 30, 5, screenWidth / 2, screenHeight / 2);
    hero.xvelocity = 5;
    hero.yvelocity = 5;

    std::vector<Ball> balls;
    std::vector<Ball> cornerBalls;
    double lastTime = GetTime();
    bool isGameOver = false;

    cornerBalls.push_back(Ball(screenWidth, screenHeight, 20, WHITE, 100, 100)); // Top-left
    cornerBalls.push_back(Ball(screenWidth, screenHeight, 20, WHITE, 100, 100)); // Top-right
    cornerBalls.push_back(Ball(screenWidth, screenHeight, 20, WHITE, 100, 100)); // Bottom-left
    cornerBalls.push_back(Ball(screenWidth, screenHeight, 20, WHITE, 100, 100)); // Bottom-right

    Rectangle retryButton = {screenWidth / 2 - 100, screenHeight / 2 + 50, 200, 50};

    double stateTime = GetTime();
    bool isYellow = false;
    bool canDelete = false;

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

        if (isGameOver) {
            DrawText("GAME OVER!", screenWidth / 2 - 100, screenHeight / 2 - 100, 50, RED);
            DrawText(TextFormat("Score: %i", score), screenWidth / 2 - 50, screenHeight / 2, 30, WHITE);

            if (CheckCollisionPointRec(GetMousePosition(), retryButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                // Reset the game state
                isGameOver = false;
                score = 0;

                // Clear existing balls and recreate corner balls
                balls.clear();
                cornerBalls.clear();
                cornerBalls.push_back(Ball(screenWidth, screenHeight, 20, WHITE, 100, 100)); // Top-left
                cornerBalls.push_back(Ball(screenWidth, screenHeight, 20, WHITE, 100, 100)); // Top-right
                cornerBalls.push_back(Ball(screenWidth, screenHeight, 20, WHITE, 100, 100)); // Bottom-left
                cornerBalls.push_back(Ball(screenWidth, screenHeight, 20, WHITE, 100, 100)); // Bottom-right

                // Reset hero position and states
                hero.centerX = screenWidth / 2;
                hero.centerY = screenHeight / 2;
                stateTime = GetTime();
                isYellow = false;
                canDelete = false;
            }

            DrawRectangleRec(retryButton, DARKGREEN);
            DrawText("Retry", screenWidth / 2 - 30, screenHeight / 2 + 60, 30, WHITE);
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

    CloseWindow();
    return 0;
}
