#include "PaintView.h"
#include "../raygui.h"
#include "../tinyfiledialogs.h"
#include <filesystem>

namespace {
    // Paleta oscura neutra, con un azul accesible como color de acción.
    constexpr Color kAppBg      = { 15, 17, 21, 255 };
    constexpr Color kSurface    = { 24, 27, 33, 255 };
    constexpr Color kSurfaceAlt = { 31, 35, 43, 255 };
    constexpr Color kBorder     = { 51, 57, 68, 255 };
    constexpr Color kText       = { 235, 238, 244, 255 };
    constexpr Color kMuted      = { 154, 163, 177, 255 };
    constexpr Color kAccent     = { 77, 143, 255, 255 };
    constexpr Color kAccentSoft = { 41, 70, 119, 255 };
    constexpr Color kDanger     = { 239, 102, 110, 255 };

    float ClampValue(float value, float minimum, float maximum) {
        return value < minimum ? minimum : (value > maximum ? maximum : value);
    }
}

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

    m_iconFont = LoadFontEx("../assets/remixicon.ttf", 24, iconCodepoints, 8);
    if (m_iconFont.texture.id != 0) {
        SetTextureFilter(m_iconFont.texture, TEXTURE_FILTER_BILINEAR);
    }

    GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, 0x1f232bff);
    GuiSetStyle(DEFAULT, BASE_COLOR_FOCUSED, 0x303846ff);
    GuiSetStyle(DEFAULT, BASE_COLOR_PRESSED, 0x4d8fffFF);
    GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, 0x333944ff);
    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, 0xebeef4ff);
}

PaintView::~PaintView() {
    if (m_font.texture.id != 0) UnloadFont(m_font);
    if (m_iconFont.texture.id != 0) UnloadFont(m_iconFont);
}

void PaintView::Render() {
    BeginDrawing();

    const int screenW = GetScreenWidth();
    const int screenH = GetScreenHeight();
    const float topBarH = 64.0f;
    const float bottomBarH = 28.0f;
    const float leftPanelW = 82.0f;
    const float rightPanelW = ClampValue(screenW * 0.235f, 270.0f, 330.0f);
    const float rightPanelX = (float)screenW - rightPanelW;
    const float contentBottom = (float)screenH - bottomBarH;
    const Color workspace = { 19, 22, 27, 255 };

    const Vector2 mousePos = GetMousePosition();
    const Color divider = { 43, 48, 58, 255 };
    const Color hover = { 42, 48, 59, 255 };
    const Color disabled = { 74, 80, 92, 255 };
    const Color white = { 248, 249, 252, 255 };
    const char* activeTooltip = nullptr;

    auto DrawLabel = [&](const char* text, Vector2 position, float size, Color color) {
        DrawTextEx(m_font, text, position, size, 1.0f, color);
    };

    auto DrawCenteredLabel = [&](const char* text, Rectangle rect, float size, Color color) {
        Vector2 measured = MeasureTextEx(m_font, text, size, 1.0f);
        DrawLabel(text,
                  { rect.x + (rect.width - measured.x) * 0.5f,
                    rect.y + (rect.height - measured.y) * 0.5f - 1.0f },
                  size, color);
    };

    auto DrawPanel = [&](Rectangle rect, Color color) {
        DrawRectangleRec(rect, color);
        DrawRectangleLinesEx(rect, 1.0f, divider);
    };

    auto DrawActionButton = [&](Rectangle rect, const char* text, bool isHovered,
                                bool primary, bool danger = false) {
        Color fill = primary ? kAccent : (isHovered ? hover : kSurfaceAlt);
        Color label = primary ? white : (danger ? kDanger : kText);
        DrawRectangleRounded(rect, 0.18f, 6, fill);
        if (!primary) DrawRectangleLinesEx(rect, 1.0f, divider);
        DrawCenteredLabel(text, rect, 13.0f, label);
    };

    // Fondo del espacio de trabajo.
    ClearBackground(kAppBg);
    DrawRectangle(0, 0, screenW, screenH, workspace);

    // Barra superior: navegación clara, acciones agrupadas y estado del motor.
    DrawRectangle(0, 0, screenW, (int)topBarH, kSurface);
    DrawLine(0, (int)topBarH, screenW, (int)topBarH, divider);
    DrawLabel("PAINT", { 22, 14 }, 22.0f, white);
    DrawLabel("Estudio de dibujo", { 23, 38 }, 10.0f, kMuted);

    const char* topBtns[3] = { "Limpiar", "Guardar", "Cargar" };
    const char* topTooltips[3] = {
        "Limpiar todo el lienzo", "Guardar archivo (.rpnt)", "Cargar archivo (.rpnt)"
    };
    const float topStartX = 174.0f;
    for (int i = 0; i < 3; i++) {
        Rectangle btnRect = { topStartX + (i * 86.0f), 14.0f, 78.0f, 36.0f };
        bool isHovered = CheckCollisionPointRec(mousePos, btnRect);
        DrawActionButton(btnRect, topBtns[i], isHovered, i == 1, false);
        if (isHovered) activeTooltip = topTooltips[i];

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
                const char* savePath = tinyfd_saveFileDialog(
                    "Guardar Dibujo", defaultPath, 1, filters, "Archivo Raylib Paint (.rpnt)");
                if (savePath != nullptr) m_document->SaveToFile(savePath);
            }

            if (i == 2) {
                const char* filters[1] = { "*.rpnt" };
                const char* defaultPath = TextFormat("%sdraws/", GetApplicationDirectory());
                const char* loadPath = tinyfd_openFileDialog(
                    "Cargar Dibujo", defaultPath, 1, filters,
                    "Archivo Raylib Paint (.rpnt)", 0);
                if (loadPath != nullptr) m_document->LoadFromFile(loadPath);
            }
        }
    }

    // Selector del motor de renderizado en la barra superior.
    bool optimized = (m_document->GetDrawingMode() == MODE_OPTIMIZED);
    Rectangle renderRect = { (float)screenW - rightPanelW - 190.0f, 14.0f, 176.0f, 36.0f };
    bool renderHovered = CheckCollisionPointRec(mousePos, renderRect);
    DrawRectangleRounded(renderRect, 0.18f, 6, renderHovered ? hover : kSurfaceAlt);
    DrawRectangleLinesEx(renderRect, 1.0f, divider);
    DrawLabel("Motor", { renderRect.x + 12, renderRect.y + 11 }, 12.0f, kMuted);
    DrawCenteredLabel(optimized ? "OPTIMIZADO" : "PÍXEL A PÍXEL",
                      { renderRect.x + 62, renderRect.y, 108, renderRect.height },
                      11.0f, optimized ? kAccent : kText);
    if (renderHovered) activeTooltip = "Cambiar motor de renderizado";
    if (renderHovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        m_document->SetDrawingMode(optimized ? MODE_PIXEL_BY_PIXEL : MODE_OPTIMIZED);
    }

    // Panel izquierdo: herramientas con iconos y estados visuales consistentes.
    DrawPanel({ 0, topBarH, leftPanelW, contentBottom - topBarH }, kSurface);
    DrawLabel("HERRAMIENTAS", { 13, topBarH + 18 }, 9.0f, kMuted);

    const char* toolTooltips[8] = {
        "Seleccionar / Mover", "Borrar Figura", "Línea", "Círculo",
        "Elipse", "Rectángulo", "Triángulo", "Curva Bezier"
    };
    int iconCodepoints[8] = {
        0xEC0A, 0xEC9F, 0xF1AF, 0xF3C1,
        0xEB37, 0xF3D6, 0xF3E4, 0xF69D
    };

    for (int i = 0; i < 8; i++) {
        Rectangle btnRect = { 15.0f, topBarH + 43.0f + (i * 54.0f), 52.0f, 46.0f };
        bool isHovered = CheckCollisionPointRec(mousePos, btnRect);
        bool isActive = false;
        if (i == 0) isActive = (m_document->GetCurrentTool() == SHAPE_NONE);
        else if (i >= 2) isActive = (m_document->GetCurrentTool() == (ShapeType)(i - 2));

        if (isActive) {
            DrawRectangleRounded(btnRect, 0.18f, 6, kAccentSoft);
            DrawRectangleRec({ btnRect.x, btnRect.y + 8, 3, btnRect.height - 16 }, kAccent);
        } else if (isHovered) {
            DrawRectangleRounded(btnRect, 0.18f, 6, hover);
        }

        if (isHovered) activeTooltip = toolTooltips[i];
        if (isHovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            if (i == 0) {
                m_document->SetCurrentTool(SHAPE_NONE);
            } else if (i == 1) {
                Shape* selected = m_document->GetSelectedShape();
                if (selected) m_document->RemoveShape(selected);
            } else {
                if (m_document->GetCurrentTool() == (ShapeType)(i - 2))
                    m_document->SetCurrentTool(SHAPE_NONE);
                else
                    m_document->SetCurrentTool((ShapeType)(i - 2));
            }
        }

        int bytesResult = 0;
        const char* iconStr = CodepointToUTF8(iconCodepoints[i], &bytesResult);
        Vector2 iconSize = MeasureTextEx(m_iconFont, iconStr, 22.0f, 1.0f);
        Color iconColor = isActive ? white : (i == 1 ? kDanger : (isHovered ? white : kMuted));
        DrawTextEx(m_iconFont, iconStr,
                   { btnRect.x + (btnRect.width - iconSize.x) * 0.5f,
                     btnRect.y + (btnRect.height - iconSize.y) * 0.5f },
                   22.0f, 1.0f, iconColor);
    }

    // Lienzo: margen respirable, sombra y clipping para conservar el área de dibujo.
    const float canvasX = leftPanelW + 26.0f;
    const float canvasY = topBarH + 26.0f;
    const float canvasW = rightPanelX - canvasX - 26.0f;
    const float canvasH = contentBottom - canvasY - 26.0f;
    Rectangle canvas = { canvasX, canvasY, canvasW, canvasH };
    DrawRectangleRounded({ canvas.x + 7, canvas.y + 8, canvas.width, canvas.height }, 0.015f, 4,
                         Color{ 0, 0, 0, 90 });
    DrawRectangleRec(canvas, m_document->GetBackgroundColor());
    DrawRectangleLinesEx(canvas, 1.0f, Color{ 82, 89, 103, 255 });

    BeginScissorMode((int)canvas.x, (int)canvas.y, (int)canvas.width, (int)canvas.height);
    for (Shape* shape : m_document->GetShapes()) {
        if (m_document->GetDrawingMode() == MODE_OPTIMIZED) shape->RenderOptimized();
        else shape->RenderPixelByPixel();
        if (shape == m_document->GetSelectedShape()) shape->DrawSelection();
    }
    EndScissorMode();

    // Panel derecho: inspector con pestañas, picker y muestras de color.
    DrawPanel({ rightPanelX, topBarH, rightPanelW, contentBottom - topBarH }, kSurface);
    DrawLabel("PROPIEDADES", { rightPanelX + 22, topBarH + 18 }, 16.0f, white);
    DrawLabel("Apariencia del documento", { rightPanelX + 22, topBarH + 40 }, 11.0f, kMuted);

    const float tabsY = topBarH + 76.0f;
    const float tabsW = rightPanelW - 44.0f;
    const float tabW = tabsW / 3.0f;
    int editMode = m_document->GetColorEditMode();
    const char* tabNames[3] = { "Relleno", "Borde", "Fondo" };
    const char* tabTooltips[3] = { "Color de Relleno", "Color de Contorno", "Color de Fondo del Lienzo" };

    DrawRectangleRounded({ rightPanelX + 22, tabsY, tabsW, 34 }, 0.18f, 6, kSurfaceAlt);
    for (int i = 0; i < 3; i++) {
        Rectangle tabRect = { rightPanelX + 22 + (i * tabW), tabsY, tabW, 34 };
        bool isHovered = CheckCollisionPointRec(mousePos, tabRect);
        if (editMode == i) {
            DrawRectangleRounded(tabRect, 0.18f, 6, kAccent);
            DrawCenteredLabel(tabNames[i], tabRect, 12.0f, white);
        } else {
            if (isHovered) DrawRectangleRounded(tabRect, 0.18f, 6, hover);
            DrawCenteredLabel(tabNames[i], tabRect, 12.0f, isHovered ? white : kMuted);
            if (isHovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
                m_document->SetColorEditMode(i);
        }
        if (isHovered) activeTooltip = tabTooltips[i];
    }

    Color activeColor;
    if (editMode == 0) activeColor = m_document->GetCurrentFillColor();
    else if (editMode == 1) activeColor = m_document->GetCurrentBorderColor();
    else activeColor = m_document->GetBackgroundColor();

    DrawLabel("Selector de color", { rightPanelX + 22, tabsY + 56 }, 12.0f, kText);
    DrawLabel("Elige un color para la capa activa", { rightPanelX + 22, tabsY + 75 }, 10.0f, kMuted);
    GuiColorPicker({ rightPanelX + 22, tabsY + 96, rightPanelW - 44, 182 }, nullptr, &activeColor);

    if (editMode == 0) m_document->SetCurrentFillColor(activeColor);
    else if (editMode == 1) m_document->SetCurrentBorderColor(activeColor);
    else m_document->SetBackgroundColor(activeColor);

    DrawLabel("Muestras activas", { rightPanelX + 22, tabsY + 302 }, 12.0f, kText);
    const char* swatchNames[3] = { "Relleno", "Borde", "Fondo" };
    Color swatchColors[3] = {
        m_document->GetCurrentFillColor(),
        m_document->GetCurrentBorderColor(),
        m_document->GetBackgroundColor()
    };
    for (int i = 0; i < 3; i++) {
        float swatchX = rightPanelX + 22 + i * ((rightPanelW - 44) / 3.0f);
        Rectangle swatch = { swatchX, tabsY + 326, 36, 30 };
        DrawRectangleRounded(swatch, 0.16f, 5, swatchColors[i]);
        DrawRectangleLinesEx(swatch, 1.0f, white);
        DrawCenteredLabel(swatchNames[i], { swatch.x - 5, swatch.y + 34, 46, 18 }, 9.0f, kMuted);
    }

    // Barra inferior con estado contextual, útil sin recargar la interfaz.
    DrawRectangle(0, contentBottom, screenW, bottomBarH, kSurface);
    DrawLine(0, (int)contentBottom, screenW, (int)contentBottom, divider);
    const char* toolStatus = "Seleccionar / Mover";
    if (m_document->GetCurrentTool() != SHAPE_NONE) {
        static const char* shapeNames[] = { "Línea", "Círculo", "Elipse", "Rectángulo", "Triángulo", "Curva Bezier" };
        int shapeIndex = (int)m_document->GetCurrentTool();
        if (shapeIndex >= 0 && shapeIndex < 6) toolStatus = shapeNames[shapeIndex];
    }
    DrawLabel("Herramienta activa:", { 18, contentBottom + 8 }, 10.0f, kMuted);
    DrawLabel(toolStatus, { 112, contentBottom + 8 }, 10.0f, kText);
    DrawLabel(optimized ? "Render optimizado" : "Render píxel a píxel",
              { (float)screenW - 148, contentBottom + 8 }, 10.0f, optimized ? kAccent : kMuted);

    // Menú contextual flotante, ahora con jerarquía y separación visual.
    if (m_document->IsContextMenuOpen()) {
        Vector2 pos = m_document->GetContextMenuPos();
        const float menuW = 194.0f;
        const float menuH = 224.0f;
        if (pos.x + menuW > screenW - 8) pos.x = screenW - menuW - 8;
        if (pos.y + menuH > screenH - 8) pos.y = screenH - menuH - 8;

        DrawRectangleRounded({ pos.x + 6, pos.y + 7, menuW, menuH }, 0.12f, 8, Color{ 0, 0, 0, 100 });
        DrawRectangleRounded({ pos.x, pos.y, menuW, menuH }, 0.12f, 8, kSurface);
        DrawRectangleLinesEx({ pos.x, pos.y, menuW, menuH }, 1.0f, divider);
        DrawLabel("ORDEN DE CAPAS", { pos.x + 14, pos.y + 13 }, 9.0f, kMuted);

        const char* ctxItems[6] = {
            "Traer al frente", "Avanzar nivel", "Retroceder nivel",
            "Enviar al fondo", "Aplicar Relleno", "Aplicar Borde"
        };
        for (int i = 0; i < 6; i++) {
            float yOffset = pos.y + 32 + (i * 29);
            if (i > 3) yOffset += 10;
            Rectangle itemRect = { pos.x + 8, yOffset, menuW - 16, 25 };
            bool isHovered = CheckCollisionPointRec(mousePos, itemRect);
            if (isHovered) DrawRectangleRounded(itemRect, 0.18f, 5, kAccentSoft);
            DrawLabel(ctxItems[i], { itemRect.x + 9, itemRect.y + 6 }, 11.0f,
                      isHovered ? white : kText);

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
    }

    // Tooltip compacto, siempre por encima de los demás elementos.
    if (activeTooltip != nullptr && !m_document->IsContextMenuOpen()) {
        Vector2 textSize = MeasureTextEx(m_font, activeTooltip, 12.0f, 1.0f);
        Rectangle tooltip = { mousePos.x + 14, mousePos.y + 16, textSize.x + 18, textSize.y + 12 };
        if (tooltip.x + tooltip.width > screenW - 6) tooltip.x = screenW - tooltip.width - 6;
        if (tooltip.y + tooltip.height > screenH - 6) tooltip.y = screenH - tooltip.height - 6;
        DrawRectangleRounded(tooltip, 0.18f, 6, Color{ 10, 12, 16, 245 });
        DrawRectangleLinesEx(tooltip, 1.0f, divider);
        DrawLabel(activeTooltip, { tooltip.x + 9, tooltip.y + 6 }, 12.0f, white);
    }

    EndDrawing();
}
