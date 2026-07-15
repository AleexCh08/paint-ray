#include "Circle.h"

Circle::Circle(Vector2 center, float radius, Color color) 
    : m_center(center), m_radius(radius), m_color(color), m_colorBorder(BLACK) 
{
    m_type = SHAPE_CIRCLE;
}

Circle::~Circle() {}

void Circle::RenderPixelByPixel() {
    int x0 = (int)m_center.x;
    int y0 = (int)m_center.y;
    int radius = (int)m_radius;

    // Relleno original del círculo
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x * x + y * y <= radius * radius) {
                DrawPixel(x0 + x, y0 + y, m_color); 
            }
        }
    }

    // Dibujar el borde del círculo (Algoritmo original)
    int x = radius; 
    int y = 0; 
    int err = 0;
    while (x >= y) {
        DrawPixel(x0 + x, y0 + y, m_colorBorder);
        DrawPixel(x0 + y, y0 + x, m_colorBorder);
        DrawPixel(x0 - y, y0 + x, m_colorBorder);
        DrawPixel(x0 - x, y0 + y, m_colorBorder);
        DrawPixel(x0 - x, y0 - y, m_colorBorder);
        DrawPixel(x0 - y, y0 - x, m_colorBorder);
        DrawPixel(x0 + y, y0 - x, m_colorBorder);
        DrawPixel(x0 + x, y0 - y, m_colorBorder);

        if (err <= 0) {
            y += 1;
            err += 2 * y + 1;
        }
        if (err > 0) {
            x -= 1;
            err -= 2 * x + 1;
        }
    }
}

void Circle::RenderOptimized() {
    DrawCircleV(m_center, m_radius, m_color);
    DrawCircleLinesV(m_center, m_radius, m_colorBorder);
}

void Circle::DrawSelection() {
    // Puntos de control y borde rojo de selección
    float ctrlSize = 4.0f;
    DrawCircleV({m_center.x - m_radius, m_center.y}, ctrlSize, BLUE);
    DrawCircleV({m_center.x + m_radius, m_center.y}, ctrlSize, BLUE);
    DrawCircleV({m_center.x, m_center.y - m_radius}, ctrlSize, BLUE);
    DrawCircleV({m_center.x, m_center.y + m_radius}, ctrlSize, BLUE);
    
    DrawCircleLinesV(m_center, m_radius + 2.0f, RED);
}

void Circle::Move(Vector2 offset) {
    m_center.x += offset.x;
    m_center.y += offset.y;
}

bool Circle::IsPointInside(Vector2 point) {
    // Calculo original de colisión
    float dx = point.x - m_center.x;
    float dy = point.y - m_center.y;
    return (dx * dx + dy * dy) <= (m_radius * m_radius);
}

void Circle::SetColor(Color color) { m_color = color; }
Color Circle::GetColor() const { return m_color; }
void Circle::SetColorBorder(Color color) { m_colorBorder = color; }
Color Circle::GetColorBorder() const { return m_colorBorder; }
void Circle::SetRadius(float radius) { m_radius = radius; }