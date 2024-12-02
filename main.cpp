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

    // Initialize corner balls
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

            // Update all balls' speed based on hero's movement
            for (auto& ball : balls) {
                ball.adjustSpeed(newSpeed);
            }
            for (auto& cornerBall : cornerBalls) {
                cornerBall.adjustSpeed(newSpeed);
            }

            // Add new balls every 3 seconds
            double currentTime = GetTime();
            if (currentTime - lastTime >= 3.0 && !isYellow) {
                balls.push_back(Ball(screenWidth, screenHeight, 20, WHITE));
                lastTime = currentTime;
            }

            // Increase score if hero is moving
            if (hero.isMoving) {
                score++;
            }

            // Update all balls' positions
            for (auto& ball : balls) {
                ball.updatePos(screenWidth, screenHeight);
            }
            for (auto& cornerBall : cornerBalls) {
                cornerBall.updatePos(screenWidth, screenHeight);
            }

            // Check for collision between hero and any ball
            if (hero.checkCollision(balls) || hero.checkCollision(cornerBalls)) {
                isGameOver = true;  // End game if collision occurs
            }
        }

        // Handle timing and color changes
        if (GetTime() - stateTime >= 10.0 && !isYellow) {  // Set yellow time to 3 seconds
            isYellow = true;
            canDelete = true;
            // Change all balls to yellow and enable destruction
            for (auto& ball : balls) {
                ball.color = YELLOW;
                ball.setDestroyable(true);  // Enable destruction
            }
            for (auto& cornerBall : cornerBalls) {
                cornerBall.color = YELLOW;
                cornerBall.setDestroyable(true);  // Enable destruction
            }
            stateTime = GetTime();  // Reset the state time
        }

        if (GetTime() - stateTime >= 13.0 && isYellow) {  // Reset to white after 3 seconds (total 10 seconds for white)
            // Change all balls back to white and disable destruction
            for (auto& ball : balls) {
                ball.color = WHITE;
                ball.setDestroyable(false);  // Disable destruction
            }
            for (auto& cornerBall : cornerBalls) {
                cornerBall.color = WHITE;
                cornerBall.setDestroyable(false);  // Disable destruction
            }

            // Disable deletion and allow new ball spawn
            isYellow = false;
            canDelete = false;
            stateTime = GetTime();  // Reset the state time

            // Spawn a new ball after the white period
            balls.push_back(Ball(screenWidth, screenHeight, 20, WHITE));
        }

        // Delete clicked balls if they are yellow and destroyable
        if (canDelete) {
    Vector2 mousePoint = GetMousePosition();
    for (auto it = balls.begin(); it != balls.end();) {
        if (it->isClicked(mousePoint) && it->destroyable && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            it = balls.erase(it);
            score += 100;  // Add points for destroying a ball
        } else {
            ++it;
        }
    }
    for (auto it = cornerBalls.begin(); it != cornerBalls.end();) {
        if (it->isClicked(mousePoint) && it->destroyable && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            it = cornerBalls.erase(it);
            score += 5000;  // Add points for corner ball destruction
        } else {
            ++it;
        }
    }
}

        // Draw everything
        BeginDrawing();
        ClearBackground(BLACK);

        if (isGameOver) {
            DrawText("GAME OVER!", screenWidth / 2 - 100, screenHeight / 2 - 100, 50, RED);
            DrawText(TextFormat("Score: %i", score), screenWidth / 2 - 50, screenHeight / 2, 30, WHITE);

            // Retry button
            if (CheckCollisionPointRec(GetMousePosition(), retryButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                // Reset game state
                isGameOver = false;
                score = 0;
                balls.clear();
                cornerBalls.clear();
                hero.centerX = screenWidth / 2;
                hero.centerY = screenHeight / 2;
                stateTime = GetTime();  // Reset time when retrying
                isYellow = false;  // Reset ball color state
                canDelete = false;
            }
            DrawRectangleRec(retryButton, GREEN);
            DrawText("Retry", retryButton.x + 70, retryButton.y + 15, 30, WHITE);
        } else {
            // Draw balls and hero
            hero.draw();
            for (auto& ball : balls) {
                ball.draw();
            }
            for (auto& cornerBall : cornerBalls) {
                cornerBall.draw();
            }

            // Display score
            DrawText(TextFormat("Score: %i", score), 10, 10, 30, WHITE);
        }

        EndDrawing();
    }

    CloseWindow();  // Close window and OpenGL context

    return 0;
}
