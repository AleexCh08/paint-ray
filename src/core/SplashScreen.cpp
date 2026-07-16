#include "SplashScreen.h"
#include <cstddef>

void SplashScreen::Show(int screenWidth, int screenHeight) {
    Image logo = LoadImage("../assets/logo.png");
    Texture2D splashTexture = { 0 };
    
    if (logo.data != NULL) {
        splashTexture = LoadTextureFromImage(logo);
        UnloadImage(logo);
    }

    int framesCounter = 0;

    while (!WindowShouldClose()) {
        framesCounter++;
        if (framesCounter > 150 || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            break; 
        }

        BeginDrawing();
        ClearBackground(Color{ 20, 20, 22, 255 });

        if (splashTexture.id != 0) {
            int logoX = (screenWidth / 2) - (splashTexture.width / 2);
            int logoY = (screenHeight / 2) - (splashTexture.height / 2);
            DrawTexture(splashTexture, logoX, logoY, WHITE);
            DrawText("Cargando entorno", screenWidth / 2 - 70, logoY + splashTexture.height + 20, 16, LIGHTGRAY);
        }
        
        EndDrawing();
    }

    if (splashTexture.id != 0) UnloadTexture(splashTexture);
}