#include "raylib.h"
#include "models/PaintDocument.h"
#include "views/PaintView.h"
#include "controllers/PaintController.h"
#include "raymath.h" 

int main() {
    SetConfigFlags(FLAG_WINDOW_MAXIMIZED | FLAG_WINDOW_RESIZABLE);
    InitWindow(1440, 900, "Paint Ray");
    SetTargetFPS(60);

    PaintDocument model;
    PaintView view(&model);
    PaintController controller(&model);

    while (!WindowShouldClose()) {
        controller.Update();  
        view.Render();        
    }

    CloseWindow();
    return 0;
}