#include "Line.h"
#include <cmath>
#include <algorithm>

Line::Line(Vector2 start, Vector2 end, Color color) 
    : m_start(start), m_end(end), m_color(color), m_colorBorder(color), m_draggingControlPoint(-1) 
{
    m_type = SHAPE_LINE;
}

Line::~Line() {}

void Line::RenderPixelByPixel() {
    // Algoritmo de Bresenham original migrado
    int x0 = (int)m_start.x;
    int y0 = (int)m_start.y;
    int x1 = (int)m_end.x;
    int y1 = (int)m_end.y;

    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        DrawPixel(x0, y0, m_colorBorder);

        if (x0 == x1 && y0 == y1) break;

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void Line::RenderOptimized() {
    // Renderizado acelerado de Raylib
    DrawLineV(m_start, m_end, m_colorBorder);
}

void Line::DrawSelection() {
    DrawControlPoints();
    // En lugar de una línea punteada que requiere lógica compleja, 
    // la resaltamos dibujando una línea roja de fondo.
    DrawLineEx(m_start, m_end, 2.0f, RED); 
}

void Line::DrawControlPoints() {
    const float controlPointSize = 4.0f;
    // Círculos azules como puntos de control usando la función de Raylib
    DrawCircleV(m_start, controlPointSize, BLUE);
    DrawCircleV(m_end, controlPointSize, BLUE);
}

bool Line::IsPointInside(Vector2 point) {
    const float selectionThreshold = 5.0f;

    float x0 = m_start.x;
    float y0 = m_start.y;
    float x1 = m_end.x;
    float y1 = m_end.y;
    float px = point.x;
    float py = point.y;

    // Distancia perpendicular a la línea
    float numerator = std::abs((y1 - y0) * px - (x1 - x0) * py + x1 * y0 - y1 * x0);
    float denominator = std::sqrt(std::pow(y1 - y0, 2) + std::pow(x1 - x0, 2));
    float distance = numerator / denominator;

    // Validación extra: Bounding Box para que la selección solo funcione en el segmento dibujado
    float minX = std::min(x0, x1) - selectionThreshold;
    float maxX = std::max(x0, x1) + selectionThreshold;
    float minY = std::min(y0, y1) - selectionThreshold;
    float maxY = std::max(y0, y1) + selectionThreshold;

    return (distance <= selectionThreshold) && (px >= minX && px <= maxX && py >= minY && py <= maxY);
}

bool Line::IsCursorOnControlPoint(Vector2 point) const {
    const float controlPointSize = 4.0f;
    // Raylib tiene una función nativa para comprobar colisión entre un punto y un círculo
    return CheckCollisionPointCircle(point, m_start, controlPointSize) || 
           CheckCollisionPointCircle(point, m_end, controlPointSize);
}

void Line::Move(Vector2 offset) {
    m_start.x += offset.x;
    m_start.y += offset.y;
    m_end.x += offset.x;
    m_end.y += offset.y;
}

void Line::SetEnd(Vector2 end) {
    m_end = end;
}

bool Line::TryGrabControlPoint(Vector2 point) {
    const float controlPointSize = 4.0f;
    if (CheckCollisionPointCircle(point, m_start, controlPointSize)) {
        m_draggingControlPoint = 0;
        return true;
    } else if (CheckCollisionPointCircle(point, m_end, controlPointSize)) {
        m_draggingControlPoint = 1;
        return true;
    }
    return false;
}

void Line::DragControlPoint(Vector2 point) {
    if (m_draggingControlPoint == 0) m_start = point;
    else if (m_draggingControlPoint == 1) m_end = point;
}

void Line::StopDragging() {
    m_draggingControlPoint = -1;
}

void Line::SetColor(Color color) { m_color = color; }
Color Line::GetColor() const { return m_color; }

void Line::SetColorBorder(Color color) { m_colorBorder = color; }
Color Line::GetColorBorder() const { return m_colorBorder; }