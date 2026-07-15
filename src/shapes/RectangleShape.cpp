#include "RectangleShape.h"
#include <sstream>

RectangleShape::RectangleShape(Rectangle bounds, Color color) 
    : m_bounds(bounds), m_color(color), m_colorBorder(BLACK) 
{
    m_type = SHAPE_RECTANGLE;
}

RectangleShape::~RectangleShape() {}

void RectangleShape::RenderPixelByPixel() {
    int left = (int)m_bounds.x;
    int top = (int)m_bounds.y;
    int right = (int)(m_bounds.x + m_bounds.width);
    int bottom = (int)(m_bounds.y + m_bounds.height);

    for (int y = top; y <= bottom; y++) {
        for (int x = left; x <= right; x++) {
            DrawPixel(x, y, m_color);
        }
    }

    for (int x = left; x <= right; x++) {
        DrawPixel(x, top, m_colorBorder); 
        DrawPixel(x, bottom, m_colorBorder); 
    }
    for (int y = top; y <= bottom; y++) {
        DrawPixel(left, y, m_colorBorder); 
        DrawPixel(right, y, m_colorBorder); 
    }
}

void RectangleShape::RenderOptimized() {
    DrawRectangleRec(m_bounds, m_color);
    DrawRectangleLinesEx(m_bounds, 1.0f, m_colorBorder);
}

void RectangleShape::DrawSelection() {
    float ctrlSize = 4.0f;
    DrawCircleV({m_bounds.x + m_bounds.width/2, m_bounds.y}, ctrlSize, BLUE);
    DrawCircleV({m_bounds.x + m_bounds.width/2, m_bounds.y + m_bounds.height}, ctrlSize, BLUE);
    DrawCircleV({m_bounds.x, m_bounds.y + m_bounds.height/2}, ctrlSize, BLUE);
    DrawCircleV({m_bounds.x + m_bounds.width, m_bounds.y + m_bounds.height/2}, ctrlSize, BLUE);

    Rectangle selBounds = { m_bounds.x - 2, m_bounds.y - 2, m_bounds.width + 4, m_bounds.height + 4 };
    DrawRectangleLinesEx(selBounds, 1.0f, RED);
}

void RectangleShape::Move(Vector2 offset) {
    m_bounds.x += offset.x;
    m_bounds.y += offset.y;
}

bool RectangleShape::IsPointInside(Vector2 point) {
    return CheckCollisionPointRec(point, m_bounds);
}

void RectangleShape::SetColor(Color color) { m_color = color; }
Color RectangleShape::GetColor() const { return m_color; }
void RectangleShape::SetColorBorder(Color color) { m_colorBorder = color; }
Color RectangleShape::GetColorBorder() const { return m_colorBorder; }
void RectangleShape::SetBounds(Rectangle bounds) { m_bounds = bounds; }

std::string RectangleShape::Serialize() const {
    std::ostringstream oss;
    oss << "FILLED_RECTANGLE " << m_bounds.x << " " << m_bounds.y << " " << m_bounds.width << " " << m_bounds.height << " "
        << (m_colorBorder.r / 255.f) << " " << (m_colorBorder.g / 255.f) << " " << (m_colorBorder.b / 255.f) << " "
        << (m_color.r / 255.f) << " " << (m_color.g / 255.f) << " " << (m_color.b / 255.f);
    return oss.str();
}