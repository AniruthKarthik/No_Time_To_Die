#include "include/raylib.h"
#include <math.h>  // For cos and sin functions

int main() {
    const int screenWidth = 1920;
    const int screenHeight = 1080;

    InitWindow(screenWidth, screenHeight, "Centered Star");
    SetTargetFPS(60);

    // Star properties
    int points = 5;           // Number of points (5 for a 5-pointed star)
    int outerRadius = 30;    // Outer radius of the star
    int innerRadius = 3;     // Inner radius of the star

    // Calculate the center position of the screen
    float centerX = screenWidth / 2;
    float centerY = screenHeight / 2;

    // Generate the points of the star
    Vector2 vertices[points * 2];  // We need twice the points for the inner and outer vertices

    for (int i = 0; i < points * 2; i++) {
        float angle = (2.0f * PI / points) * i;  // Angle for each vertex

        // Alternate between the inner and outer radius
        float radius = (i % 2 == 0) ? outerRadius : innerRadius;

        vertices[i] = {centerX + radius * cos(angle), centerY + radius * sin(angle)};
    }

    while (!WindowShouldClose()) {
        BeginDrawing();
        
        ClearBackground(BLACK);

        // Draw the star by connecting the vertices in sequence
        for (int i = 0; i < points * 2; i++) {
            int nextIndex = (i + 1) % (points * 2);  // Wrap around to the start
            DrawLineV(vertices[i], vertices[nextIndex], WHITE);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
