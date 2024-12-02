#include "include/raylib.h"
#include <math.h>
#include <cstdlib>
#include <ctime>
#include <vector>

class Enemy {
public:
    int x, y;  // Change back to int if that's what your original code used
    int xspeed, yspeed;
    int radius;
    Color color;

    Enemy(int maxX, int maxY, int r, Color col) {
        // Ensure ball spawns fully inside screen
        radius = r;
        x = radius + rand() % (maxX - 2 * radius);
        y = radius + rand() % (maxY - 2 * radius);
        
        // Random initial direction
        xspeed = (rand() % 2 == 0 ? 1 : -1) * (5 + rand() % 10);
        yspeed = (rand() % 2 == 0 ? 1 : -1) * (5 + rand() % 10);
        
        color = col;
    }

    void drawball() {
        DrawCircle(x, y, radius, color);
    }

    void adjustSpeed(int newSpeed) {
        // Simple speed adjustment
        xspeed = (xspeed > 0 ? 1 : -1) * newSpeed;
        yspeed = (yspeed > 0 ? 1 : -1) * newSpeed;
    }

    void updatePos(int screenWidth, int screenHeight) {
        // Update position
        x += xspeed;
        y += yspeed;

        // Bounce horizontally
        if (x - radius <= 0) {
            x = radius;
            xspeed = abs(xspeed);  // Ensure moving right
        }
        else if (x + radius >= screenWidth) {
            x = screenWidth - radius;
            xspeed = -abs(xspeed);  // Ensure moving left
        }

        // Bounce vertically
        if (y - radius <= 0) {
            y = radius;
            yspeed = abs(yspeed);  // Ensure moving down
        }
        else if (y + radius >= screenHeight) {
            y = screenHeight - radius;
            yspeed = -abs(yspeed);  // Ensure moving up
        }
    }
};

class Hero {
public:
    int points, outerRadius, innerRadius;
    float centerX, centerY, xvelocity, yvelocity;
    Vector2 vertices[10];
    bool isMoving = false;  // Flag to track if the hero is moving

    Hero(int p, int oRadius, int iRadius, float cX, float cY)
        : points(p), outerRadius(oRadius), innerRadius(iRadius), centerX(cX), centerY(cY), xvelocity(0), yvelocity(0) {
        calculateVertices();
    }

    void calculateVertices() {
        for (int i = 0; i < points * 2; i++) {
            float angle = (2.0f * PI / points) * i;
            float radius = (i % 2 == 0) ? outerRadius : innerRadius;
            vertices[i] = {centerX + radius * cos(angle), centerY + radius * sin(angle)};
        }
    }

    void drawStar() {
        for (int i = 0; i < points * 2; i++) {
            int nextIndex = (i + 1) % (points * 2);
            DrawLineV(vertices[i], vertices[nextIndex], WHITE);
        }
    }

    void updatePos() {
        // Track if the hero is moving
        isMoving = false;

        if (IsKeyDown(KEY_W) && centerY - outerRadius > 0) {
            centerY -= yvelocity;
            isMoving = true;  // Hero is moving
        }
        if (IsKeyDown(KEY_S) && centerY + outerRadius < GetScreenHeight()) {
            centerY += yvelocity;
            isMoving = true;  // Hero is moving
        }
        if (IsKeyDown(KEY_A) && centerX - outerRadius > 0) {
            centerX -= xvelocity;
            isMoving = true;  // Hero is moving
        }
        if (IsKeyDown(KEY_D) && centerX + outerRadius < GetScreenWidth()) {
            centerX += xvelocity;
            isMoving = true;  // Hero is moving
        }

        // Recalculate vertices with updated position
        calculateVertices();
    }
};

int main() {
    srand(time(0));
    const int screenWidth = 1920;
    const int screenHeight = 1080;

    // Window properties
    InitWindow(screenWidth, screenHeight, "No time to die");
    SetTargetFPS(60);

    // Hero object
    Hero star(5, 30, 5, screenWidth / 2, screenHeight / 2);
    star.xvelocity = 5;
    star.yvelocity = 5;

    std::vector<Enemy> enemies;  // Vector to store all generated balls

    double lastime = GetTime();
    double currenttime = lastime;

    while (!WindowShouldClose()) {
        // Update hero position
        star.updatePos();

        // Bounce off screen edges for the hero
        

        // Adjust enemy speed based on hero's movement
        int newSpeed = (star.isMoving) ? 15 : 4;  // Increase speed when hero is moving
        for (auto& enemy : enemies) {
            enemy.adjustSpeed(newSpeed);  // Update speed of all enemies
        }

        // Check if 3 seconds have passed to create a new ball
        currenttime = GetTime();
        if (currenttime - lastime >= 3.0) {
            // Add a new enemy (ball) with random coordinates and properties
            enemies.push_back(Enemy(screenWidth, screenHeight, 20.0f, WHITE));
            lastime = currenttime;  // Update lastime to the current time after generating a new ball
        }

        // Update each enemy position and bounce off the screen edges
        for (auto& enemy : enemies) {
            enemy.updatePos(screenWidth, screenHeight);  // Update enemy position and check for bouncing
            enemy.drawball();  // Draw the enemy ball
        }

        BeginDrawing();

        ClearBackground(BLACK);
        star.drawStar();  // Draw the hero

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
