#include "raylib.h"
#include "raymath.h"
#include "models/PaintDocument.h"
#include "views/PaintView.h"
#include "controllers/PaintController.h"
#include "core/SplashScreen.h"
 
int main() { 
    SetConfigFlags(FLAG_WINDOW_MAXIMIZED | FLAG_WINDOW_RESIZABLE);   
    const int screenWidth = 1440;
    const int screenHeight = 900;  
    InitWindow(screenWidth, screenHeight, "PaintRay");
    MaximizeWindow();

    Image logo = LoadImage("../assets/logo.png");
    if (logo.data != NULL) {
        SetWindowIcon(logo); 
        UnloadImage(logo);
    }

    SetTargetFPS(60);

    bool showSplashScreen = true;
    int framesCounter = 0;

    PaintDocument model;
    PaintView view(&model);
    PaintController controller(&model);

    SplashScreen::Show(screenWidth, screenHeight);
    while (!WindowShouldClose()) {
        controller.Update(); 
        view.Render();
    }

    CloseWindow();
    return 0;
}