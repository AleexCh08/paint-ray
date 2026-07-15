#include "PaintView.h"
#include "../raygui.h"

PaintView::PaintView(PaintDocument* document) : m_document(document) {
    GuiSetStyle(DEFAULT, TEXT_SIZE, 14);
}

void PaintView::Render() {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    for (Shape* shape : m_document->GetShapes()) {
        if (m_document->GetDrawingMode() == MODE_OPTIMIZED) {
            shape->RenderOptimized();
        } else {
            shape->RenderPixelByPixel();
        }
        
        if (shape == m_document->GetSelectedShape()) {
            shape->DrawSelection();
        }
    }

    DrawRectangle(0, 0, GetScreenWidth(), 40, {245, 245, 245, 255});
    DrawLine(0, 40, GetScreenWidth(), 40, LIGHTGRAY);

    if (GuiButton({ 10, 8, 100, 24 }, "Limpiar Todo")) {
        m_document->ClearAll();
    }
    
    if (GuiButton({ 120, 8, 100, 24 }, "Borrar Figura")) {
        Shape* selected = m_document->GetSelectedShape();
        if (selected) m_document->RemoveShape(selected);
    }

    bool isOptimized = (m_document->GetDrawingMode() == MODE_OPTIMIZED);
    
    if (GuiCheckBox({ 240, 10, 20, 20 }, "Render Optimizado", &isOptimized)) {
        m_document->SetDrawingMode(isOptimized ? MODE_OPTIMIZED : MODE_PIXEL_BY_PIXEL);
    }

    DrawRectangle(0, 40, 80, GetScreenHeight() - 40, {240, 240, 240, 255});
    DrawLine(80, 40, 80, GetScreenHeight(), LIGHTGRAY);

    GuiLabel({ 10, 50, 60, 20 }, "HERRAM.");
    const char* toolNames[] = { "Linea", "Circulo", "Elipse", "Rect.", "Triang.", "Bezier" };
    
    for (int i = 0; i < 6; i++) {
        bool isActive = (m_document->GetCurrentTool() == (ShapeType)i);
        Rectangle btnRect = { 10.0f, 80.0f + (i * 35), 60.0f, 30.0f };
        
        if (isActive) {
            DrawRectangleRec({btnRect.x - 2, btnRect.y - 2, btnRect.width + 4, btnRect.height + 4}, DARKGRAY);
        }

        if (GuiButton(btnRect, toolNames[i])) {
            if (m_document->GetCurrentTool() == (ShapeType)i) {
                m_document->SetCurrentTool(SHAPE_NONE);
            } else {
                m_document->SetCurrentTool((ShapeType)i);
            }
        }
    }


    GuiLabel({ 10, 310, 60, 20 }, "COLOR");
    
    Color palette[] = { BLACK, RED, BLUE, GREEN, ORANGE, PURPLE };
    for(int i = 0; i < 6; i++) {
        int col = i % 2;
        int row = i / 2;
        Rectangle btnRect = { 10.0f + col * 32.0f, 340.0f + row * 32.0f, 28.0f, 28.0f };
        
        if (m_document->GetCurrentColor().r == palette[i].r && 
            m_document->GetCurrentColor().g == palette[i].g && 
            m_document->GetCurrentColor().b == palette[i].b) {
            DrawRectangleRec({btnRect.x - 2, btnRect.y - 2, btnRect.width + 4, btnRect.height + 4}, DARKGRAY);
        }

        if (GuiButton(btnRect, "")) {
            m_document->SetCurrentColor(palette[i]);
        }
        DrawRectangleRec({btnRect.x + 2, btnRect.y + 2, btnRect.width - 4, btnRect.height - 4}, palette[i]);
    }

    EndDrawing();
}