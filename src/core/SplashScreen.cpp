#include "SplashScreen.h"
#include <cstddef>

void SplashScreen::Show(int screenWidth, int screenHeight) {
    Image logo = LoadImage("../assets/logo.png");
    Texture2D splashTexture = { 0 };
    
    if (logo.data != NULL) {
        splashTexture = LoadTextureFromImage(logo);
        UnloadImage(logo);
    }

    Font splashFont = LoadFontEx("../assets/Roboto-Regular.ttf", 32, 0, 250);
    if (splashFont.texture.id != 0) {
        SetTextureFilter(splashFont.texture, TEXTURE_FILTER_BILINEAR);
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

            const char* loadingText = "Cargando entorno...";
            Vector2 textSize = MeasureTextEx(splashFont, loadingText, 16, 1);
            float textX = (screenWidth / 2.0f) - (textSize.x / 2.0f);           
            DrawTextEx(splashFont, loadingText, { textX, (float)(logoY + splashTexture.height + 20) }, 16, 1, LIGHTGRAY);
        }
        
        EndDrawing();
    }

    if (splashTexture.id != 0) UnloadTexture(splashTexture);
    if (splashFont.texture.id != 0) UnloadFont(splashFont);
}