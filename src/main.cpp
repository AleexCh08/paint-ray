#include "raylib.h"
#include "models/PaintDocument.h"
#include "views/PaintView.h"
#include "controllers/PaintController.h"
#include "raymath.h" 

int main() {
    InitWindow(1280, 720, "Raylib Paint Clone");
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