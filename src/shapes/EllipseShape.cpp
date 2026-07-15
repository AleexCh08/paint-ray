#include "EllipseShape.h"

EllipseShape::EllipseShape(Rectangle bounds, Color color) 
    : m_bounds(bounds), m_color(color), m_colorBorder(BLACK) 
{
    m_type = SHAPE_ELLIPSE;
}

EllipseShape::~EllipseShape() {}

void EllipseShape::RenderPixelByPixel() {
    int x0 = (int)(m_bounds.x + (m_bounds.width / 2));
    int y0 = (int)(m_bounds.y + (m_bounds.height / 2));
    int a = (int)(m_bounds.width / 2);
    int b = (int)(m_bounds.height / 2);
    int x = 0;
    int y = b;
    int a2 = a * a;
    int b2 = b * b;
    int fa2 = 4 * a2, fb2 = 4 * b2;
    int sigma = 2 * b2 + a2 * (1 - 2 * b);

    auto drawEllipsePoints = [&](int px, int py, Color color) {
        DrawPixel(x0 + px, y0 + py, color);
        DrawPixel(x0 - px, y0 + py, color);
        DrawPixel(x0 + px, y0 - py, color);
        DrawPixel(x0 - px, y0 - py, color);
    };

    drawEllipsePoints(x, y, m_colorBorder);

    // Región 1 original
    while (b2 * (2 * x + 1) < a2 * (2 * y - 1)) {
        if (sigma >= 0) {
            sigma += fa2 * (1 - y);
            y--;
        }
        sigma += b2 * ((4 * x) + 6);
        x++;
        drawEllipsePoints(x, y, m_colorBorder);
    }

    // Región 2 original
    sigma = b2 * ((2 * x + 1) * (2 * x + 1)) + a2 * ((2 * y - 1) * (2 * y - 1)) - a2 * b2;
    while (y >= 0) {
        if (sigma <= 0) {
            sigma += b2 * ((4 * x) + 6);
            x++;
        }
        sigma += a2 * (3 - 2 * y);
        y--;
        drawEllipsePoints(x, y, m_colorBorder);
    }

    // Relleno original
    for (int iy = -b; iy <= b; iy++) {
        for (int ix = -a; ix <= a; ix++) {
            if ((ix * ix * b2 + iy * iy * a2) <= (a2 * b2)) {
                DrawPixel(x0 + ix, y0 + iy, m_color); 
            }
        }
    }
}

void EllipseShape::RenderOptimized() {
    int centerX = (int)(m_bounds.x + m_bounds.width / 2);
    int centerY = (int)(m_bounds.y + m_bounds.height / 2);
    float radiusH = m_bounds.width / 2.0f;
    float radiusV = m_bounds.height / 2.0f;

    DrawEllipse(centerX, centerY, radiusH, radiusV, m_color);
    DrawEllipseLines(centerX, centerY, radiusH, radiusV, m_colorBorder);
}

void EllipseShape::DrawSelection() {
    float ctrlSize = 4.0f;
    DrawCircleV({m_bounds.x, m_bounds.y + m_bounds.height / 2}, ctrlSize, BLUE);
    DrawCircleV({m_bounds.x + m_bounds.width, m_bounds.y + m_bounds.height / 2}, ctrlSize, BLUE);
    DrawCircleV({m_bounds.x + m_bounds.width / 2, m_bounds.y}, ctrlSize, BLUE);
    DrawCircleV({m_bounds.x + m_bounds.width / 2, m_bounds.y + m_bounds.height}, ctrlSize, BLUE);

    int centerX = (int)(m_bounds.x + m_bounds.width / 2);
    int centerY = (int)(m_bounds.y + m_bounds.height / 2);
    DrawEllipseLines(centerX, centerY, m_bounds.width / 2.0f + 2.0f, m_bounds.height / 2.0f + 2.0f, RED);
}

void EllipseShape::Move(Vector2 offset) {
    m_bounds.x += offset.x;
    m_bounds.y += offset.y;
}

bool EllipseShape::IsPointInside(Vector2 point) {
    double centerX = m_bounds.x + m_bounds.width / 2.0; 
    double centerY = m_bounds.y + m_bounds.height / 2.0; 
    double radiusX = m_bounds.width / 2.0; 
    double radiusY = m_bounds.height / 2.0; 
    double dx = point.x - centerX; 
    double dy = point.y - centerY; 
    return (dx * dx) / (radiusX * radiusX) + (dy * dy) / (radiusY * radiusY) <= 1.0;
}

void EllipseShape::SetColor(Color color) { m_color = color; }
Color EllipseShape::GetColor() const { return m_color; }
void EllipseShape::SetColorBorder(Color color) { m_colorBorder = color; }
Color EllipseShape::GetColorBorder() const { return m_colorBorder; }
void EllipseShape::SetBounds(Rectangle bounds) { m_bounds = bounds; }