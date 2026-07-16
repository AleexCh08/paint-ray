#include "Circle.h"
#include <sstream>
#include <raymath.h>

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

    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x * x + y * y <= radius * radius) {
                DrawPixel(x0 + x, y0 + y, m_color); 
            }
        }
    }

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

void Circle::Move(Vector2 offset) {
    m_center.x += offset.x;
    m_center.y += offset.y;
}

bool Circle::IsPointInside(Vector2 point) {
    float dx = point.x - m_center.x;
    float dy = point.y - m_center.y;
    return (dx * dx + dy * dy) <= (m_radius * m_radius);
}

void Circle::SetColor(Color color) { m_color = color; }
Color Circle::GetColor() const { return m_color; }
void Circle::SetColorBorder(Color color) { m_colorBorder = color; }
Color Circle::GetColorBorder() const { return m_colorBorder; }
void Circle::SetRadius(float radius) { m_radius = radius; }

std::string Circle::Serialize() const {
    std::ostringstream oss;
    oss << "FILLED_CIRCLE " << m_center.x << " " << m_center.y << " " << (m_center.x + m_radius) << " " << m_center.y << " "
        << (m_colorBorder.r / 255.f) << " " << (m_colorBorder.g / 255.f) << " " << (m_colorBorder.b / 255.f) << " "
        << (m_color.r / 255.f) << " " << (m_color.g / 255.f) << " " << (m_color.b / 255.f);
    return oss.str();
}

void Circle::DrawSelection() {
    DrawCircleLines(m_center.x, m_center.y, m_radius + 2, RED);
    DrawCircleV({m_center.x + m_radius, m_center.y}, 4.0f, BLUE);
}

bool Circle::TryGrabControlPoint(Vector2 point) {
    if (CheckCollisionPointCircle(point, {m_center.x + m_radius, m_center.y}, 8.0f)) {
        m_draggingControlPoint = 0;
        return true;
    }
    return false;
}

void Circle::DragControlPoint(Vector2 point) {
    if (m_draggingControlPoint == 0) {
        // Al arrastrar el punto, la distancia desde el centro se convierte en el nuevo radio
        m_radius = Vector2Distance(m_center, point);
    }
}

void Circle::StopDragging() { m_draggingControlPoint = -1; }