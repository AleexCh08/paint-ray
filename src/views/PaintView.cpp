#include "PaintView.h"
#include "../raygui.h"

PaintView::PaintView(PaintDocument* document) : m_document(document) {
    // Forzar Modo Oscuro Profesional alterando los estilos base de Raygui
    GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, 0x2b2b2bff);
    GuiSetStyle(DEFAULT, BASE_COLOR_FOCUSED, 0x4a4a4aff);
    GuiSetStyle(DEFAULT, BASE_COLOR_PRESSED, 0x8a8a8aff);
    GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, 0x1f1f1fff);
    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, 0xffffffff);
    GuiSetStyle(DEFAULT, TEXT_COLOR_FOCUSED, 0xffffffff);
    GuiSetStyle(DEFAULT, TEXT_COLOR_PRESSED, 0xffffffff);
}

void PaintView::Render() {
    BeginDrawing();
    ClearBackground(Color{ 35, 35, 35, 255 }); 

    // --- 1. LIENZO DINÁMICO (CANVAS) ---
    // Calculado dinámicamente para llenar el centro de la pantalla
    Rectangle canvas = { 280, 60, (float)GetScreenWidth() - 400, (float)GetScreenHeight() - 80 };
    
    // Gris muy suave en lugar de blanco hiriente para no encandilar
    DrawRectangleRec(canvas, Color{ 210, 210, 210, 255 }); 
    DrawRectangleLinesEx(canvas, 2.0f, BLACK); 

    BeginScissorMode(canvas.x, canvas.y, canvas.width, canvas.height);
    for (Shape* shape : m_document->GetShapes()) {
        if (m_document->GetDrawingMode() == MODE_OPTIMIZED) shape->RenderOptimized();
        else shape->RenderPixelByPixel();
        if (shape == m_document->GetSelectedShape()) shape->DrawSelection();
    }
    EndScissorMode();

    // --- 2. PANEL IZQUIERDO (Colores) ---
    DrawRectangle(0, 40, 260, GetScreenHeight() - 40, Color{ 45, 45, 45, 255 });
    GuiLabel({ 20, 50, 100, 20 }, "PALETA DE COLORES");
    
    Color palette[] = { 
        BLACK, DARKGRAY, GRAY, LIGHTGRAY, WHITE,
        RED, MAROON, ORANGE, GOLD, YELLOW,
        LIME, GREEN, DARKGREEN, SKYBLUE, BLUE,
        DARKBLUE, PURPLE, VIOLET, PINK, MAGENTA
    };
    
    int cols = 5;
    for(int i = 0; i < 20; i++) {
        int col = i % cols;
        int row = i / cols;
        Rectangle btnRect = { 20.0f + col * 42.0f, 80.0f + row * 42.0f, 35.0f, 35.0f };
        
        if (m_document->GetCurrentColor().r == palette[i].r && 
            m_document->GetCurrentColor().g == palette[i].g && 
            m_document->GetCurrentColor().b == palette[i].b) {
            DrawRectangleRec({btnRect.x - 3, btnRect.y - 3, btnRect.width + 6, btnRect.height + 6}, LIGHTGRAY);
        }

        if (GuiButton(btnRect, "")) m_document->SetCurrentColor(palette[i]);
        DrawRectangleRec({btnRect.x + 2, btnRect.y + 2, btnRect.width - 4, btnRect.height - 4}, palette[i]);
    }

    // --- 3. PANEL DERECHO DINÁMICO (Herramientas) ---
    float rightPanelX = (float)GetScreenWidth() - 100.0f;
    DrawRectangle(rightPanelX, 40, 100, GetScreenHeight() - 40, Color{ 45, 45, 45, 255 });
    
    for (int i = 0; i < 6; i++) {
        bool isActive = (m_document->GetCurrentTool() == (ShapeType)i);
        Rectangle btnRect = { rightPanelX + 20.0f, 60.0f + (i * 65), 60.0f, 50.0f };
        
        if (isActive) DrawRectangleRec({btnRect.x - 2, btnRect.y - 2, btnRect.width + 4, btnRect.height + 4}, SKYBLUE);

        if (GuiButton(btnRect, "")) {
            if (m_document->GetCurrentTool() == (ShapeType)i) m_document->SetCurrentTool(SHAPE_NONE);
            else m_document->SetCurrentTool((ShapeType)i);
        }

        Vector2 center = { btnRect.x + btnRect.width/2, btnRect.y + btnRect.height/2 };
        Color iconColor = isActive ? WHITE : LIGHTGRAY;
        
        switch(i) {
            case 0: DrawLineEx({center.x - 15, center.y + 15}, {center.x + 15, center.y - 15}, 3.0f, iconColor); break;
            case 1: DrawCircleLines(center.x, center.y, 12.0f, iconColor); break;
            case 2: DrawEllipseLines(center.x, center.y, 18.0f, 10.0f, iconColor); break;
            case 3: DrawRectangleLinesEx({center.x - 15, center.y - 12, 30, 24}, 2.0f, iconColor); break;
            case 4: DrawTriangleLines({center.x, center.y - 15}, {center.x - 15, center.y + 12}, {center.x + 15, center.y + 12}, iconColor); break;
            case 5: DrawSplineSegmentBezierCubic({center.x - 15, center.y + 10}, {center.x - 5, center.y - 15}, {center.x + 5, center.y + 15}, {center.x + 15, center.y - 10}, 2.0f, iconColor); break; 
        }
    }

    // --- 4. BARRA SUPERIOR GLOBALES ---
    DrawRectangle(0, 0, GetScreenWidth(), 40, Color{ 25, 25, 25, 255 });
    
    if (GuiButton({ 10, 8, 120, 24 }, "Limpiar Lienzo")) m_document->ClearAll();
    
    if (GuiButton({ 140, 8, 120, 24 }, "Borrar Figura")) {
        Shape* selected = m_document->GetSelectedShape();
        if (selected) m_document->RemoveShape(selected);
    }

    bool isOpt = (m_document->GetDrawingMode() == MODE_OPTIMIZED);
    if (GuiButton({ 280, 8, 180, 24 }, isOpt ? "Render: OPTIMIZADO" : "Render: PIXEL-PIXEL")) {
        m_document->SetDrawingMode(isOpt ? MODE_PIXEL_BY_PIXEL : MODE_OPTIMIZED);
    }

    EndDrawing();
}