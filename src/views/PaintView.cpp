#include "PaintView.h"
#include "../raygui.h"

PaintView::PaintView(PaintDocument* document) : m_document(document) {
    m_font = LoadFontEx("../assets/Roboto-Regular.ttf", 20, 0, 250);
    if (m_font.texture.id != 0) {
        GuiSetFont(m_font);
        GuiSetStyle(DEFAULT, TEXT_SIZE, 14);
    }

    // Estilo base solo para el ColorPicker, el resto es 100% custom
    GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, 0x2d2d30ff);
    GuiSetStyle(DEFAULT, BASE_COLOR_FOCUSED, 0x3e3e42ff);
    GuiSetStyle(DEFAULT, BASE_COLOR_PRESSED, 0x007accff);
    GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, 0x3e3e42ff);
    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, 0xccccccff);
}

PaintView::~PaintView() {
    if (m_font.texture.id != 0) UnloadFont(m_font);
}

void PaintView::Render() {
    BeginDrawing();
    ClearBackground(Color{ 20, 20, 22, 255 }); // Entorno oscuro (Workspace)

    float topBarH = 45.0f;
    float leftPanelW = 64.0f;
    float rightPanelW = 280.0f;
    Color panelColor = { 30, 30, 34, 255 }; 
    Color accentColor = { 0, 122, 204, 255 }; 
    Color hoverColor = { 60, 60, 65, 255 };

    Vector2 mousePos = GetMousePosition();

    // --- 1. LIENZO CENTRAL (ARTBOARD) ---
    Rectangle canvas = { leftPanelW + 20, topBarH + 20, (float)GetScreenWidth() - leftPanelW - rightPanelW - 40, (float)GetScreenHeight() - topBarH - 40 };
    
    // Sombra del lienzo
    DrawRectangleRec({canvas.x + 5, canvas.y + 5, canvas.width, canvas.height}, Color{ 0, 0, 0, 80 });
    DrawRectangleRec(canvas, m_document->GetBackgroundColor()); 

    BeginScissorMode((int)canvas.x, (int)canvas.y, (int)canvas.width, (int)canvas.height);
    for (Shape* shape : m_document->GetShapes()) {
        if (m_document->GetDrawingMode() == MODE_OPTIMIZED) shape->RenderOptimized();
        else shape->RenderPixelByPixel();
        if (shape == m_document->GetSelectedShape()) shape->DrawSelection();
    }
    EndScissorMode();

    // --- 2. PANEL IZQUIERDO (HERRAMIENTAS FLAT) ---
    DrawRectangle(0, topBarH, leftPanelW, GetScreenHeight() - topBarH, panelColor);
    DrawLine(leftPanelW, topBarH, leftPanelW, GetScreenHeight(), Color{ 45, 45, 50, 255 });
    
    for (int i = 0; i < 8; i++) {
        bool isActive = false;
        if (i == 0) isActive = (m_document->GetCurrentTool() == SHAPE_NONE);
        else if (i == 1) isActive = false; 
        else isActive = (m_document->GetCurrentTool() == (ShapeType)(i - 2));

        Rectangle btnRect = { 8.0f, 60.0f + (i * 54), 48.0f, 48.0f };
        bool isHovered = CheckCollisionPointRec(mousePos, btnRect);

        // Interfaz programada a mano (Sin GuiButton)
        if (isActive) DrawRectangleRounded(btnRect, 0.2f, 4, accentColor);
        else if (isHovered) DrawRectangleRounded(btnRect, 0.2f, 4, hoverColor);

        // Lógica de clics pura
        if (isHovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            if (i == 0) {
                m_document->SetCurrentTool(SHAPE_NONE);
            } else if (i == 1) {
                Shape* selected = m_document->GetSelectedShape();
                if (selected) m_document->RemoveShape(selected);
            } else {
                if (m_document->GetCurrentTool() == (ShapeType)(i - 2)) m_document->SetCurrentTool(SHAPE_NONE);
                else m_document->SetCurrentTool((ShapeType)(i - 2));
            }
        }

        Vector2 center = { btnRect.x + btnRect.width/2, btnRect.y + btnRect.height/2 };
        Color iconColor = isActive ? WHITE : (isHovered ? WHITE : Color{ 170, 170, 175, 255 });
        
        switch(i) {
            case 0: 
                DrawTriangle({center.x - 6, center.y - 10}, {center.x - 6, center.y + 10}, {center.x + 8, center.y + 2}, iconColor);
                DrawLineEx({center.x - 2, center.y + 4}, {center.x + 4, center.y + 14}, 2.5f, iconColor);
                break;
            case 1: 
                DrawRectangleRec({center.x - 8, center.y - 10, 16, 2}, Color{ 220, 80, 80, 255 });
                DrawRectangleRec({center.x - 3, center.y - 13, 6, 3}, Color{ 220, 80, 80, 255 });
                DrawRectangleLinesEx({center.x - 6, center.y - 8, 12, 18}, 2.0f, Color{ 220, 80, 80, 255 });
                DrawLineEx({center.x - 2, center.y - 4}, {center.x - 2, center.y + 6}, 2.0f, Color{ 220, 80, 80, 255 });
                DrawLineEx({center.x + 2, center.y - 4}, {center.x + 2, center.y + 6}, 2.0f, Color{ 220, 80, 80, 255 });
                break;
            case 2: 
                DrawLineEx({center.x - 10, center.y + 10}, {center.x + 10, center.y - 10}, 2.0f, iconColor); 
                DrawCircleV({center.x - 10, center.y + 10}, 2.5f, iconColor);
                DrawCircleV({center.x + 10, center.y - 10}, 2.5f, iconColor);
                break;
            case 3: 
                DrawCircleLines((int)center.x, (int)center.y, 12.0f, iconColor); 
                DrawCircleV(center, 2.0f, iconColor);
                break;
            case 4: 
                DrawEllipseLines((int)center.x, (int)center.y, 16.0f, 9.0f, iconColor); 
                DrawCircleV(center, 2.0f, iconColor);
                break;
            case 5: 
                DrawRectangleLinesEx({center.x - 12, center.y - 10, 24, 20}, 2.0f, iconColor); 
                break;
            case 6: 
                DrawTriangleLines({center.x, center.y - 10}, {center.x - 12, center.y + 10}, {center.x + 12, center.y + 10}, iconColor); 
                break;
            case 7: 
                DrawLineEx({center.x - 12, center.y + 8}, {center.x - 4, center.y - 12}, 1.0f, ColorAlpha(iconColor, 0.4f));
                DrawLineEx({center.x + 12, center.y - 8}, {center.x + 4, center.y + 12}, 1.0f, ColorAlpha(iconColor, 0.4f));
                DrawSplineSegmentBezierCubic({center.x - 12, center.y + 8}, {center.x - 4, center.y - 12}, {center.x + 4, center.y + 12}, {center.x + 12, center.y - 8}, 2.0f, iconColor); 
                DrawCircleV({center.x - 12, center.y + 8}, 2.0f, iconColor);
                DrawCircleV({center.x + 12, center.y - 8}, 2.0f, iconColor);
                break; 
        }
    }

    // --- 3. PANEL DERECHO (INSPECTOR MODERNO) ---
    float rightPanelX = GetScreenWidth() - rightPanelW;
    DrawRectangle(rightPanelX, topBarH, rightPanelW, GetScreenHeight() - topBarH, panelColor);
    DrawLine(rightPanelX, topBarH, rightPanelX, GetScreenHeight(), Color{ 45, 45, 50, 255 });
    
    DrawTextEx(m_font, "PROPIEDADES", { rightPanelX + 20, topBarH + 20 }, 16, 1, WHITE);
    DrawLine(rightPanelX + 20, topBarH + 45, GetScreenWidth() - 20, topBarH + 45, Color{ 60, 60, 65, 255 });

    int editMode = m_document->GetColorEditMode();
    
    // Pestañas (Tabs) Planas
    const char* tabNames[3] = {"Relleno", "Borde", "Fondo"};
    for (int i = 0; i < 3; i++) {
        Rectangle tabRect = { rightPanelX + 20 + (i * 75), topBarH + 60, 70, 30 };
        bool isTabHovered = CheckCollisionPointRec(mousePos, tabRect);
        
        if (editMode == i) {
            DrawRectangleRounded(tabRect, 0.2f, 4, accentColor);
            DrawTextEx(m_font, tabNames[i], { tabRect.x + 12, tabRect.y + 7 }, 14, 1, WHITE);
        } else {
            if (isTabHovered) DrawRectangleRounded(tabRect, 0.2f, 4, hoverColor);
            DrawTextEx(m_font, tabNames[i], { tabRect.x + 12, tabRect.y + 7 }, 14, 1, Color{ 170, 170, 175, 255 });
            if (isTabHovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) m_document->SetColorEditMode(i);
        }
    }

    Color activeColor;
    if (editMode == 0) activeColor = m_document->GetCurrentFillColor();
    else if (editMode == 1) activeColor = m_document->GetCurrentBorderColor();
    else activeColor = m_document->GetBackgroundColor();

    // El Color Picker se mantiene por utilidad, pero estilizado
    GuiColorPicker({ rightPanelX + 20, topBarH + 110, 190, 190 }, nullptr, &activeColor);

    if (editMode == 0) m_document->SetCurrentFillColor(activeColor);
    else if (editMode == 1) m_document->SetCurrentBorderColor(activeColor);
    else m_document->SetBackgroundColor(activeColor);

    // Indicadores Visuales Limpios
    DrawTextEx(m_font, "Colores Activos:", { rightPanelX + 20, topBarH + 340 }, 14, 1, LIGHTGRAY);
    DrawRectangleRounded({ rightPanelX + 20, topBarH + 370, 30, 30 }, 0.2f, 4, m_document->GetCurrentFillColor());
    DrawRectangleLinesEx({ rightPanelX + 20, topBarH + 370, 30, 30 }, 1.0f, WHITE);
    
    DrawRectangleRounded({ rightPanelX + 65, topBarH + 370, 30, 30 }, 0.2f, 4, m_document->GetCurrentBorderColor());
    DrawRectangleLinesEx({ rightPanelX + 65, topBarH + 370, 30, 30 }, 1.0f, WHITE);
    
    DrawRectangleRounded({ rightPanelX + 110, topBarH + 370, 30, 30 }, 0.2f, 4, m_document->GetBackgroundColor());
    DrawRectangleLinesEx({ rightPanelX + 110, topBarH + 370, 30, 30 }, 1.0f, WHITE);

    // --- 4. BARRA SUPERIOR (GLOBALES) ---
    DrawRectangle(0, 0, GetScreenWidth(), topBarH, Color{ 45, 45, 48, 255 }); 
    DrawLine(0, topBarH, GetScreenWidth(), topBarH, Color{ 20, 20, 20, 255 });

    const char* topBtns[4] = {"Limpiar", "Guardar", "Cargar", ""};
    for(int i=0; i<4; i++) {
        if (i == 3) { 
            bool isOpt = (m_document->GetDrawingMode() == MODE_OPTIMIZED);
            const char* text = isOpt ? "Render: OPT" : "Render: PXP";
            Rectangle btnRect = { 240.0f, 7.0f, 100.0f, 30.0f };
            bool isHovered = CheckCollisionPointRec(mousePos, btnRect);
            
            if (isHovered) DrawRectangleRounded(btnRect, 0.2f, 4, hoverColor);
            DrawTextEx(m_font, text, { btnRect.x + 10, btnRect.y + 7 }, 14, 1, isHovered ? WHITE : LIGHTGRAY);
            
            if (isHovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                m_document->SetDrawingMode(isOpt ? MODE_PIXEL_BY_PIXEL : MODE_OPTIMIZED);
            }
        } else {
            Rectangle btnRect = { 10.0f + (i * 75), 7.0f, 70.0f, 30.0f };
            bool isHovered = CheckCollisionPointRec(mousePos, btnRect);
            
            if (isHovered) DrawRectangleRounded(btnRect, 0.2f, 4, hoverColor);
            DrawTextEx(m_font, topBtns[i], { btnRect.x + 12, btnRect.y + 7 }, 14, 1, isHovered ? WHITE : LIGHTGRAY);
            
            if (isHovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                if (i == 0) m_document->ClearAll();
                if (i == 1) m_document->SaveToFile("dibujo.rpnt");
                if (i == 2) m_document->LoadFromFile("dibujo.rpnt");
            }
        }
    }

    // --- 5. MENÚ CONTEXTUAL ---
    if (m_document->IsContextMenuOpen()) {
        Vector2 pos = m_document->GetContextMenuPos();
        
        // Sombra y Fondo del panel flotante
        DrawRectangleRounded({ pos.x + 4, pos.y + 4, 150, 198 }, 0.1f, 4, Color{ 0, 0, 0, 80 });
        DrawRectangleRounded({ pos.x, pos.y, 150, 198 }, 0.1f, 4, panelColor);
        DrawRectangleLinesEx({ pos.x, pos.y, 150, 198 }, 1.0f, accentColor); 

        const char* ctxItems[6] = {"Traer al frente", "Avanzar nivel", "Retroceder nivel", "Enviar al fondo", "Aplicar Relleno", "Aplicar Borde"};
        
        for(int i=0; i<6; i++) {
            float yOffset = pos.y + 5 + (i * 30);
            if (i > 3) yOffset += 8; // Separador

            Rectangle itemRect = { pos.x + 5, yOffset, 140, 26 };
            bool isHovered = CheckCollisionPointRec(mousePos, itemRect);
            
            if (isHovered) DrawRectangleRounded(itemRect, 0.2f, 4, accentColor);
            DrawTextEx(m_font, ctxItems[i], { itemRect.x + 10, itemRect.y + 6 }, 14, 1, isHovered ? WHITE : LIGHTGRAY);

            if (isHovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                Shape* selected = m_document->GetSelectedShape();
                if (i == 0) m_document->MoveShapeToFront(selected);
                else if (i == 1) m_document->MoveShapeForward(selected);
                else if (i == 2) m_document->MoveShapeBackward(selected);
                else if (i == 3) m_document->MoveShapeToBack(selected);
                else if (i == 4 && selected) selected->SetColor(m_document->GetCurrentFillColor());
                else if (i == 5 && selected) selected->SetColorBorder(m_document->GetCurrentBorderColor());
                
                m_document->SetContextMenuState(false);
            }
        }
        DrawLine((int)pos.x + 10, (int)pos.y + 130, (int)pos.x + 140, (int)pos.y + 130, Color{ 60, 60, 65, 255 });
    }
    EndDrawing();
}