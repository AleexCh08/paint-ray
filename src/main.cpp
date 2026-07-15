#include "raylib.h"
#include "models/PaintDocument.h"
#include "views/PaintView.h"
#include "controllers/PaintController.h"
#include "raymath.h" // Para funciones matemáticas internas

int main() {
    // 1. Inicialización
    InitWindow(1280, 720, "Raylib Paint Clone");
    SetTargetFPS(60);

    // 2. Arquitectura MVC
    PaintDocument model;
    PaintView view(&model);
    PaintController controller(&model);

    // 3. Bucle Principal
    while (!WindowShouldClose()) {
        controller.Update();  // Procesa eventos
        view.Render();        // Dibuja en pantalla
    }

    // 4. Limpieza (El destructor de PaintDocument limpia la memoria automáticamente)
    CloseWindow();
    return 0;
}