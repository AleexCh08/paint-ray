#include "Triangle.h"
#include <cmath>
#include <algorithm>

Triangle::Triangle(Vector2 p1, Vector2 p2, Vector2 p3, Color color) 
    : m_color(color), m_colorBorder(BLACK) 
{
    m_points[0] = p1;
    m_points[1] = p2;
    m_points[2] = p3;
    m_type = SHAPE_TRIANGLE;
}

Triangle::~Triangle() {}

void Triangle::RenderPixelByPixel() {
    int x0 = (int)m_points[0].x, y0 = (int)m_points[0].y;
    int x1 = (int)m_points[1].x, y1 = (int)m_points[1].y;
    int x2 = (int)m_points[2].x, y2 = (int)m_points[2].y;

    auto drawLine = [&](int xStart, int yStart, int xEnd, int yEnd, Color color) {
        int dx = std::abs(xEnd - xStart);
        int dy = std::abs(yEnd - yStart);
        int sx = (xStart < xEnd) ? 1 : -1;
        int sy = (yStart < yEnd) ? 1 : -1;
        int err = dx - dy;

        while (true) {
            DrawPixel(xStart, yStart, color);
            if (xStart == xEnd && yStart == yEnd) break;
            int e2 = 2 * err;
            if (e2 > -dy) { err -= dy; xStart += sx; }
            if (e2 < dx) { err += dx; yStart += sy; }
        }
    };

    // Bordes originales
    drawLine(x0, y0, x1, y1, m_colorBorder);
    drawLine(x1, y1, x2, y2, m_colorBorder);
    drawLine(x2, y2, x0, y0, m_colorBorder);

    // Relleno original mediante Edge Function
    auto edgeFunction = [](Vector2 p0, Vector2 p1, Vector2 p) {
        return (p.x - p0.x) * (p1.y - p0.y) - (p.y - p0.y) * (p1.x - p0.x);
    };

    int minX = std::min({x0, x1, x2});
    int minY = std::min({y0, y1, y2});
    int maxX = std::max({x0, x1, x2});
    int maxY = std::max({y0, y1, y2});

    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            Vector2 p = { (float)x, (float)y };
            int w0 = (int)edgeFunction(m_points[1], m_points[2], p);
            int w1 = (int)edgeFunction(m_points[2], m_points[0], p);
            int w2 = (int)edgeFunction(m_points[0], m_points[1], p);

            if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                DrawPixel(x, y, m_color); 
            }
        }
    }
}

void Triangle::RenderOptimized() {
    // Raylib requiere orden contrario a las agujas del reloj en algunos contextos, 
    // pero DrawTriangle nativo se maneja bien pasándole los puntos directamente.
    DrawTriangle(m_points[0], m_points[1], m_points[2], m_color);
    DrawTriangleLines(m_points[0], m_points[1], m_points[2], m_colorBorder);
}

void Triangle::DrawSelection() {
    float ctrlSize = 4.0f;
    DrawCircleV(m_points[0], ctrlSize, BLUE);
    DrawCircleV(m_points[1], ctrlSize, BLUE);
    DrawCircleV(m_points[2], ctrlSize, BLUE);
    
    DrawTriangleLines(m_points[0], m_points[1], m_points[2], RED);
}

void Triangle::Move(Vector2 offset) {
    m_points[0].x += offset.x; m_points[0].y += offset.y;
    m_points[1].x += offset.x; m_points[1].y += offset.y;
    m_points[2].x += offset.x; m_points[2].y += offset.y;
}

bool Triangle::IsPointInside(Vector2 point) {
    int a = (int)((m_points[0].x - point.x) * (m_points[1].y - m_points[0].y) - (m_points[1].x - m_points[0].x) * (m_points[0].y - point.y)); 
    int b = (int)((m_points[1].x - point.x) * (m_points[2].y - m_points[1].y) - (m_points[2].x - m_points[1].x) * (m_points[1].y - point.y)); 
    int c = (int)((m_points[2].x - point.x) * (m_points[0].y - m_points[2].y) - (m_points[0].x - m_points[2].x) * (m_points[2].y - point.y)); 
    return (a >= 0 && b >= 0 && c >= 0) || (a <= 0 && b <= 0 && c <= 0); 
}

void Triangle::SetColor(Color color) { m_color = color; }
Color Triangle::GetColor() const { return m_color; }
void Triangle::SetColorBorder(Color color) { m_colorBorder = color; }
Color Triangle::GetColorBorder() const { return m_colorBorder; }

void Triangle::SetPoints(Vector2 p1, Vector2 p2, Vector2 p3) {
    m_points[0] = p1; m_points[1] = p2; m_points[2] = p3;
}