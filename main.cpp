#include "include/raylib.h"
#include <math.h>
#include <cstdlib>
#include <ctime>
#include <vector>

class Enemy {
public:
    int x, y, xspeed = 10, yspeed = 10;
    float radius;
    Color color;

    Enemy(int maxX, int maxY, float r, Color col) {
        // Generate random coordinates
        x = rand() % maxX;
        y = rand() % maxY;
        radius = r;
        color = col;
    }

    void drawball() {
        // Draw the ball at the generated coordinates
        DrawCircle(x, y, radius, color);
    }

    void update() {
        // Update position with speed
        x += xspeed;
        y += yspeed;

        // Bounce off screen edges
        if (x - radius < 0 || x + radius > GetScreenWidth()) {
            xspeed = -xspeed;
        }
        if (y - radius < 0 || y + radius > GetScreenHeight()) {
            yspeed = -yspeed;
        }
    }
};

class Hero {
public:
    int points, outerRadius, innerRadius;
    float centerX, centerY, xvelocity, yvelocity;
    Vector2 vertices[10];

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

    void updatepos() {
        if (IsKeyDown(KEY_W) && centerY - outerRadius > 0) {
            centerY -= yvelocity;
        }
        if (IsKeyDown(KEY_S) && centerY + outerRadius < GetScreenHeight()) {
            centerY += yvelocity;
        }
        if (IsKeyDown(KEY_A) && centerX - outerRadius > 0) {
            centerX -= xvelocity;
        }
        if (IsKeyDown(KEY_D) && centerX + outerRadius < GetScreenWidth()) {
            centerX += xvelocity;
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
        star.updatepos();

        // Bounce off screen edges for the hero
        if (star.centerX - star.outerRadius < 0 || star.centerX + star.outerRadius > screenWidth) {
            star.xvelocity = -star.xvelocity;
        }
        if (star.centerY - star.outerRadius < 0 || star.centerY + star.outerRadius > screenHeight) {
            star.yvelocity = -star.yvelocity;
        }

        // Check if 3 seconds have passed to create a new ball
        currenttime = GetTime();
        if (currenttime - lastime >= 3.0) {
            // Add a new enemy (ball) with random coordinates and properties
            enemies.push_back(Enemy(screenWidth, screenHeight, 20.0f, WHITE));
            lastime = currenttime;  // Update lastime to the current time after generating a new ball
        }

        BeginDrawing();

        ClearBackground(BLACK);
        star.drawStar();

        // Draw all balls (enemies)
        for (auto& enemy : enemies) {
            enemy.drawball();
            enemy.update();  // Update each enemy ball position and check for bouncing
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
