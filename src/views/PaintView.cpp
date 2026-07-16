#include "PaintView.h"
#include "../raygui.h"

PaintView::PaintView(PaintDocument* document) : m_document(document) {
    m_font = LoadFontEx("assets/Roboto-Regular.ttf", 20, 0, 250);
    if (m_font.texture.id != 0) {
        GuiSetFont(m_font);
        GuiSetStyle(DEFAULT, TEXT_SIZE, 16);
    } else {
        GuiSetStyle(DEFAULT, TEXT_SIZE, 12); 
    }

    GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, 0x282a36ff);   // Fondo paneles
    GuiSetStyle(DEFAULT, BASE_COLOR_FOCUSED, 0x44475aff);  // Hover botones
    GuiSetStyle(DEFAULT, BASE_COLOR_PRESSED, 0xbd93f9ff);  // Presionado (Acento morado)
    
    GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, 0x6272a4ff); // Bordes tenues
    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, 0xf8f8f2ff);   // Texto blanco puro
    GuiSetStyle(DEFAULT, TEXT_COLOR_FOCUSED, 0xffffffff);
    GuiSetStyle(DEFAULT, TEXT_COLOR_PRESSED, 0x282a36ff);  // Texto oscuro al presionar
    
    GuiSetStyle(BUTTON, BORDER_WIDTH, 1);
}

PaintView::~PaintView() {
    if (m_font.texture.id != 0) UnloadFont(m_font);
}

void PaintView::Render() {
    BeginDrawing();
    ClearBackground(Color{ 30, 31, 41, 255 }); 

    // --- 1. LIENZO CENTRAL ---
    Rectangle canvas = { 280, 50, (float)GetScreenWidth() - 400, (float)GetScreenHeight() - 70 };
    DrawRectangleRec(canvas, m_document->GetBackgroundColor()); 
    DrawRectangleLinesEx(canvas, 2.0f, BLACK); 

    BeginScissorMode(canvas.x, canvas.y, canvas.width, canvas.height);
    for (Shape* shape : m_document->GetShapes()) {
        if (m_document->GetDrawingMode() == MODE_OPTIMIZED) shape->RenderOptimized();
        else shape->RenderPixelByPixel();
        if (shape == m_document->GetSelectedShape()) shape->DrawSelection();
    }
    EndScissorMode();

    // --- 2. PANEL IZQUIERDO (COLORES) ---
    DrawRectangle(0, 40, 260, GetScreenHeight() - 40, Color{ 40, 42, 54, 255 });
    GuiLabel({ 20, 50, 220, 20 }, "PROPIEDADES DE COLOR");

    int editMode = m_document->GetColorEditMode();
    Color activeColor;
    
    if (editMode == 0) activeColor = m_document->GetCurrentFillColor();
    else if (editMode == 1) activeColor = m_document->GetCurrentBorderColor();
    else activeColor = m_document->GetBackgroundColor();

    GuiColorPicker({ 20, 80, 160, 160 }, nullptr, &activeColor);

    if (editMode == 0) m_document->SetCurrentFillColor(activeColor);
    else if (editMode == 1) m_document->SetCurrentBorderColor(activeColor);
    else m_document->SetBackgroundColor(activeColor);

    Rectangle rectFill =   { 20, 260, 70, 45 };
    Rectangle rectBorder = { 95, 260, 70, 45 };
    Rectangle rectBg =     { 170, 260, 70, 45 };

    Color accentColor = { 139, 233, 253, 255 }; 
    if (editMode == 0) DrawRectangleRec({rectFill.x - 2, rectFill.y - 2, rectFill.width + 4, rectFill.height + 4}, accentColor);
    else if (editMode == 1) DrawRectangleRec({rectBorder.x - 2, rectBorder.y - 2, rectBorder.width + 4, rectBorder.height + 4}, accentColor);
    else if (editMode == 2) DrawRectangleRec({rectBg.x - 2, rectBg.y - 2, rectBg.width + 4, rectBg.height + 4}, accentColor);
    
    if (GuiButton(rectFill, "")) m_document->SetColorEditMode(0);
    if (GuiButton(rectBorder, "")) m_document->SetColorEditMode(1);
    if (GuiButton(rectBg, "")) m_document->SetColorEditMode(2);

    int textY = 268;
    DrawText("Relleno", (int)rectFill.x + 14, textY, 10, WHITE);
    DrawText("Contorno", (int)rectBorder.x + 11, textY, 10, WHITE);
    DrawText("Fondo", (int)rectBg.x + 20, textY, 10, WHITE);

    DrawRectangle((int)rectFill.x + 10, (int)rectFill.y + 24, 50, 14, m_document->GetCurrentFillColor());
    DrawRectangleLines((int)rectFill.x + 10, (int)rectFill.y + 24, 50, 14, LIGHTGRAY);
    DrawRectangle((int)rectBorder.x + 10, (int)rectBorder.y + 24, 50, 14, m_document->GetCurrentBorderColor());
    DrawRectangleLines((int)rectBorder.x + 10, (int)rectBorder.y + 24, 50, 14, LIGHTGRAY);
    DrawRectangle((int)rectBg.x + 10, (int)rectBg.y + 24, 50, 14, m_document->GetBackgroundColor());
    DrawRectangleLines((int)rectBg.x + 10, (int)rectBg.y + 24, 50, 14, LIGHTGRAY);

    // --- 3. PANEL DERECHO (HERRAMIENTAS CON CURSOR Y BORRADOR) ---
    float rightPanelX = (float)GetScreenWidth() - 100.0f;
    DrawRectangle(rightPanelX, 40, 100, GetScreenHeight() - 40, Color{ 40, 42, 54, 255 });
    
    // Bucle de 8 herramientas (2 utilidades + 6 formas)
    for (int i = 0; i < 8; i++) {
        bool isActive = false;
        if (i == 0) isActive = (m_document->GetCurrentTool() == SHAPE_NONE);
        else if (i == 1) isActive = false; // El borrador es un botón de acción, no mantiene estado
        else isActive = (m_document->GetCurrentTool() == (ShapeType)(i - 2));

        Rectangle btnRect = { rightPanelX + 20.0f, 60.0f + (i * 60), 60.0f, 50.0f };
        
        if (isActive) DrawRectangleRec({btnRect.x - 2, btnRect.y - 2, btnRect.width + 4, btnRect.height + 4}, accentColor);

        if (GuiButton(btnRect, "")) {
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
        Color iconColor = isActive ? WHITE : LIGHTGRAY;
        
        // Dibujado de Iconos Vectoriales
        switch(i) {
            case 0: // Flecha de Cursor
                DrawTriangle({center.x - 8, center.y - 12}, {center.x - 8, center.y + 12}, {center.x + 8, center.y + 2}, iconColor);
                DrawLineEx({center.x - 2, center.y + 5}, {center.x + 6, center.y + 16}, 3.0f, iconColor);
                break;
            case 1: // Borrador / Cruz
                DrawLineEx({center.x - 10, center.y - 10}, {center.x + 10, center.y + 10}, 4.0f, RED);
                DrawLineEx({center.x + 10, center.y - 10}, {center.x - 10, center.y + 10}, 4.0f, RED);
                break;
            case 2: DrawLineEx({center.x - 15, center.y + 15}, {center.x + 15, center.y - 15}, 3.0f, iconColor); break;
            case 3: DrawCircleLines(center.x, center.y, 12.0f, iconColor); break;
            case 4: DrawEllipseLines(center.x, center.y, 18.0f, 10.0f, iconColor); break;
            case 5: DrawRectangleLinesEx({center.x - 15, center.y - 12, 30, 24}, 2.0f, iconColor); break;
            case 6: DrawTriangleLines({center.x, center.y - 15}, {center.x - 15, center.y + 12}, {center.x + 15, center.y + 12}, iconColor); break;
            case 7: DrawSplineSegmentBezierCubic({center.x - 15, center.y + 10}, {center.x - 5, center.y - 15}, {center.x + 5, center.y + 15}, {center.x + 15, center.y - 10}, 2.0f, iconColor); break; 
        }
    }

    // --- 4. BARRA SUPERIOR (GLOBALES CON ICONOS) ---
    DrawRectangle(0, 0, GetScreenWidth(), 40, Color{ 30, 31, 41, 255 });

    // Iconos de Raygui (#143# = Papelera, #02# = Guardar, #05# = Abrir Carpeta)
    if (GuiButton({ 10, 8, 40, 24 }, "#143#")) m_document->ClearAll();
    if (GuiButton({ 60, 8, 40, 24 }, "#02#")) m_document->SaveToFile("dibujo.rpnt");
    if (GuiButton({ 110, 8, 40, 24 }, "#05#")) m_document->LoadFromFile("dibujo.rpnt");

    // Toggle de Renderizado con icono
    bool isOpt = (m_document->GetDrawingMode() == MODE_OPTIMIZED);
    if (GuiButton({ 160, 8, 160, 24 }, isOpt ? "#112# RENDER: OPT" : "#112# RENDER: PXP")) {
        m_document->SetDrawingMode(isOpt ? MODE_PIXEL_BY_PIXEL : MODE_OPTIMIZED);
    }

    // --- 5. MENÚ CONTEXTUAL ---
    if (m_document->IsContextMenuOpen()) {
        Vector2 pos = m_document->GetContextMenuPos();
        
        DrawRectangle((int)pos.x, (int)pos.y, 140, 184, Color{ 40, 42, 54, 255 });
        DrawRectangleLines((int)pos.x, (int)pos.y, 140, 184, accentColor); // Borde brillante

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
        DrawLine((int)pos.x + 5, (int)pos.y + 118, (int)pos.x + 135, (int)pos.y + 118, GRAY);

        if (GuiButton({ pos.x + 5, pos.y + 126, 130, 24 }, "Aplicar Relleno")) {
            Shape* selected = m_document->GetSelectedShape();
            if (selected) selected->SetColor(m_document->GetCurrentFillColor());
            m_document->SetContextMenuState(false);
        }
        if (GuiButton({ pos.x + 5, pos.y + 154, 130, 24 }, "Aplicar Contorno")) {
            Shape* selected = m_document->GetSelectedShape();
            if (selected) selected->SetColorBorder(m_document->GetCurrentBorderColor());
            m_document->SetContextMenuState(false);
        }
    }
    EndDrawing();
}