#include "PaintView.h"
#include "../raygui.h"

PaintView::PaintView(PaintDocument* document) : m_document(document) {

    GuiSetStyle(DEFAULT, TEXT_SIZE, 10); 
    GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, 0x333333ff);
    GuiSetStyle(DEFAULT, BASE_COLOR_FOCUSED, 0x5a5a5aff);
    GuiSetStyle(DEFAULT, BASE_COLOR_PRESSED, 0x1a1a1aff);
    
    GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, 0x666666ff);
    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, 0xffffffff); 
    GuiSetStyle(DEFAULT, TEXT_COLOR_FOCUSED, 0xffffffff);
    GuiSetStyle(DEFAULT, TEXT_COLOR_PRESSED, 0xbbbbbbff); 
    
    GuiSetStyle(BUTTON, BORDER_WIDTH, 1);
}

void PaintView::Render() {
    BeginDrawing();
    ClearBackground(Color{ 35, 35, 35, 255 }); 

    Rectangle canvas = { 280, 60, (float)GetScreenWidth() - 400, (float)GetScreenHeight() - 80 };
    
    DrawRectangleRec(canvas, Color{ 210, 210, 210, 255 }); 
    DrawRectangleLinesEx(canvas, 2.0f, BLACK); 

    BeginScissorMode(canvas.x, canvas.y, canvas.width, canvas.height);
    for (Shape* shape : m_document->GetShapes()) {
        if (m_document->GetDrawingMode() == MODE_OPTIMIZED) shape->RenderOptimized();
        else shape->RenderPixelByPixel();
        if (shape == m_document->GetSelectedShape()) shape->DrawSelection();
    }
    EndScissorMode();

    DrawRectangle(0, 40, 260, GetScreenHeight() - 40, Color{ 45, 45, 45, 255 });
    GuiLabel({ 20, 50, 220, 20 }, "PROPIEDADES DE COLOR");

    bool editBorder = m_document->IsEditBorderMode();
    Color activeColor = editBorder ? m_document->GetCurrentBorderColor() : m_document->GetCurrentFillColor();
    GuiColorPicker({ 20, 80, 160, 160 }, nullptr, &activeColor);

    if (editBorder) {
        m_document->SetCurrentBorderColor(activeColor);
    } else {
        m_document->SetCurrentFillColor(activeColor);
    }

    Rectangle rectFill = { 20, 260, 100, 45 };
    Rectangle rectBorder = { 130, 260, 100, 45 };

    if (!editBorder) DrawRectangleRec({rectFill.x - 2, rectFill.y - 2, rectFill.width + 4, rectFill.height + 4}, SKYBLUE);
    if (editBorder) DrawRectangleRec({rectBorder.x - 2, rectBorder.y - 2, rectBorder.width + 4, rectBorder.height + 4}, SKYBLUE);
    
    if (GuiButton(rectFill, "")) m_document->SetEditBorderMode(false);
    if (GuiButton(rectBorder, "")) m_document->SetEditBorderMode(true);
    DrawText("Relleno", (int)rectFill.x + 28, (int)rectFill.y + 8, 10, WHITE);
    DrawText("Contorno", (int)rectBorder.x + 24, (int)rectBorder.y + 8, 10, WHITE);

    DrawRectangle((int)rectFill.x + 10, (int)rectFill.y + 24, 80, 14, m_document->GetCurrentFillColor());
    DrawRectangleLines((int)rectFill.x + 10, (int)rectFill.y + 24, 80, 14, LIGHTGRAY);
    DrawRectangle((int)rectBorder.x + 10, (int)rectBorder.y + 24, 80, 14, m_document->GetCurrentBorderColor());
    DrawRectangleLines((int)rectBorder.x + 10, (int)rectBorder.y + 24, 80, 14, LIGHTGRAY);

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

    if (m_document->IsContextMenuOpen()) {
        Vector2 pos = m_document->GetContextMenuPos();
        
        DrawRectangle((int)pos.x, (int)pos.y, 140, 122, Color{ 45, 45, 45, 255 });
        DrawRectangleLines((int)pos.x, (int)pos.y, 140, 122, LIGHTGRAY);

        if (GuiButton({ pos.x + 5, pos.y + 10, 130, 24 }, "Traer al frente")) {
            m_document->MoveShapeToFront(m_document->GetSelectedShape());
            m_document->SetContextMenuState(false);
        }
        if (GuiButton({ pos.x + 5, pos.y + 36, 130, 24 }, "Avanzar nivel")) {
            m_document->MoveShapeForward(m_document->GetSelectedShape());
            m_document->SetContextMenuState(false);
        }
        if (GuiButton({ pos.x + 5, pos.y + 62, 130, 24 }, "Retroceder nivel")) {
            m_document->MoveShapeBackward(m_document->GetSelectedShape());
            m_document->SetContextMenuState(false);
        }
        if (GuiButton({ pos.x + 5, pos.y + 88, 130, 24 }, "Enviar al fondo")) {
            m_document->MoveShapeToBack(m_document->GetSelectedShape());
            m_document->SetContextMenuState(false);
        }
    }
    EndDrawing();
}