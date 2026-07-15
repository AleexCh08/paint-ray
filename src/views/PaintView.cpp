#include "PaintView.h"

PaintView::PaintView(PaintDocument* document) : m_document(document) {}

void PaintView::Render() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Dibujar todas las figuras
    for (Shape* shape : m_document->GetShapes()) {
        if (m_document->GetDrawingMode() == MODE_OPTIMIZED) {
            shape->RenderOptimized();
        } else {
            shape->RenderPixelByPixel();
        }
        
        // Dibujar selección si es la figura activa
        if (shape == m_document->GetSelectedShape()) {
            shape->DrawSelection();
        }
    }

    EndDrawing();
}