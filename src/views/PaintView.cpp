#include "PaintView.h"
#include "../raygui.h"
#include "../tinyfiledialogs.h"
#include <filesystem>

PaintView::PaintView(PaintDocument* document) : m_document(document) {
    m_font = LoadFontEx("../assets/Roboto-Regular.ttf", 32, 0, 250);
    if (m_font.texture.id != 0) {
        SetTextureFilter(m_font.texture, TEXTURE_FILTER_BILINEAR);
        GuiSetFont(m_font);
        GuiSetStyle(DEFAULT, TEXT_SIZE, 14);
    }

    int iconCodepoints[8] = { 
        0xEC0A, // Mover (ri-cursor-line)
        0xEC9F, // Borrar (ri-eraser-line)
        0xF1AF, // Línea (ri-line-line)
        0xF3C1, // Círculo (ri-circle-line)
        0xEB37, // Elipse (ri-capsule-line)
        0xF3D6, // Rectángulo (ri-rectangle-line)
        0xF3E4, // Triángulo (ri-triangle-line)
        0xF69D  // Curva Bezier (ri-connector-line)
    };

    // Cargamos la fuente indicando explicitamente el arreglo de codepoints
    m_iconFont = LoadFontEx("../assets/remixicon.ttf", 24, iconCodepoints, 8);
    if (m_iconFont.texture.id != 0) {
        SetTextureFilter(m_iconFont.texture, TEXTURE_FILTER_BILINEAR);
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
    if (m_iconFont.texture.id != 0) UnloadFont(m_iconFont);
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
    const char* activeTooltip = nullptr;

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
        const char* toolTooltips[8] = {"Seleccionar / Mover", "Borrar Figura", "Línea", "Círculo", "Elipse", "Rectángulo", "Triángulo", "Curva Bezier"};
        if (isHovered) activeTooltip = toolTooltips[i];

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
        if (i == 1) iconColor = Color{ 220, 80, 80, 255 }; // El boton de borrar se mantiene rojo

        int iconCodepoints[8] = { 0xEC0A, 0xEC9F, 0xF1AF, 0xF3C1, 0xEB37, 0xF3D6, 0xF3E4, 0xF69D  };

        // Convertimos el codepoint a una cadena UTF-8 entendible por Raylib
        int bytesResult = 0;
        const char* iconStr = CodepointToUTF8(iconCodepoints[i], &bytesResult);

        // Centramos el icono vectorial dinámicamente en el botón
        Vector2 iconSize = MeasureTextEx(m_iconFont, iconStr, 22, 1);
        Vector2 iconPos = { center.x - (iconSize.x / 2.0f), center.y - (iconSize.y / 2.0f) };

        DrawTextEx(m_iconFont, iconStr, iconPos, 22, 1, iconColor);
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
        
        const char* tabTooltips[3] = {"Color de Relleno", "Color de Contorno", "Color de Fondo del Lienzo"};
        if (isTabHovered) activeTooltip = tabTooltips[i];
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
    const char* activeColorsText = "Colores Activos:";
    Vector2 textSize = MeasureTextEx(m_font, activeColorsText, 14, 1);
    float textX = rightPanelX + (rightPanelW / 2.0f) - (textSize.x / 2.0f);
    DrawTextEx(m_font, activeColorsText, { textX, topBarH + 320 }, 14, 1, LIGHTGRAY);
    float startX = rightPanelX + 80.0f;
    
    DrawRectangleRounded({ startX, topBarH + 350, 30, 30 }, 0.2f, 4, m_document->GetCurrentFillColor());
    DrawRectangleLinesEx({ startX, topBarH + 350, 30, 30 }, 1.0f, WHITE);
    
    DrawRectangleRounded({ startX + 45, topBarH + 350, 30, 30 }, 0.2f, 4, m_document->GetCurrentBorderColor());
    DrawRectangleLinesEx({ startX + 45, topBarH + 350, 30, 30 }, 1.0f, WHITE);
    
    DrawRectangleRounded({ startX + 90, topBarH + 350, 30, 30 }, 0.2f, 4, m_document->GetBackgroundColor());
    DrawRectangleLinesEx({ startX + 90, topBarH + 350, 30, 30 }, 1.0f, WHITE);

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
            
            if (isHovered) {
                DrawRectangleRounded(btnRect, 0.2f, 4, hoverColor);
                activeTooltip = "Cambiar motor de renderizado";
            }
            DrawTextEx(m_font, text, { btnRect.x + 10, btnRect.y + 7 }, 14, 1, isHovered ? WHITE : LIGHTGRAY);
            
            if (isHovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                m_document->SetDrawingMode(isOpt ? MODE_PIXEL_BY_PIXEL : MODE_OPTIMIZED);
            }
        } else {
            Rectangle btnRect = { 10.0f + (i * 75), 7.0f, 70.0f, 30.0f };
            bool isHovered = CheckCollisionPointRec(mousePos, btnRect);
            
            if (isHovered) {
                DrawRectangleRounded(btnRect, 0.2f, 4, hoverColor);
                const char* topTooltips[3] = {"Limpiar todo el lienzo", "Guardar archivo (.rpnt)", "Cargar archivo (.rpnt)"};
                activeTooltip = topTooltips[i];
            }
            DrawTextEx(m_font, topBtns[i], { btnRect.x + 12, btnRect.y + 7 }, 14, 1, isHovered ? WHITE : LIGHTGRAY);
            
            if (isHovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                if (i == 0) {
                    m_document->ClearAll();
                }

                if (i == 1 || i == 2) {
                    std::string drawsDir = TextFormat("%sdraws", GetApplicationDirectory());
                    if (!std::filesystem::exists(drawsDir)) {
                        std::filesystem::create_directory(drawsDir);
                    }
                }

                if (i == 1) {
                    const char* filters[1] = { "*.rpnt" };
                    const char* defaultPath = TextFormat("%sdraws/dibujo.rpnt", GetApplicationDirectory());
                    
                    const char* savePath = tinyfd_saveFileDialog("Guardar Dibujo", defaultPath, 1, filters, "Archivo Raylib Paint (.rpnt)");
                    if (savePath != nullptr) {
                        m_document->SaveToFile(savePath);
                    }
                }
                if (i == 2) {
                    const char* filters[1] = { "*.rpnt" };
                    const char* defaultPath = TextFormat("%sdraws/", GetApplicationDirectory());
                    
                    const char* loadPath = tinyfd_openFileDialog("Cargar Dibujo", defaultPath, 1, filters, "Archivo Raylib Paint (.rpnt)", 0);
                    if (loadPath != nullptr) {
                        m_document->LoadFromFile(loadPath);
                    }
                }
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

    // --- 6. DIBUJAR TOOLTIP (Siempre por encima de todo) ---
    if (activeTooltip != nullptr && !m_document->IsContextMenuOpen()) {
        Vector2 tSize = MeasureTextEx(m_font, activeTooltip, 14, 1);
        Rectangle tRect = { mousePos.x + 15, mousePos.y + 15, tSize.x + 16, tSize.y + 10 };
        
        if (tRect.x + tRect.width > GetScreenWidth()) tRect.x = GetScreenWidth() - tRect.width - 5;
        if (tRect.y + tRect.height > GetScreenHeight()) tRect.y = GetScreenHeight() - tRect.height - 5;

        DrawRectangleRounded(tRect, 0.2f, 4, Color{ 20, 20, 25, 230 });
        DrawRectangleLinesEx(tRect, 1.0f, Color{ 80, 80, 85, 255 });
        DrawTextEx(m_font, activeTooltip, { tRect.x + 8, tRect.y + 5 }, 14, 1, WHITE);
    }

    EndDrawing();
}