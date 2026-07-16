#include "EllipseShape.h"
#include <sstream>

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

    while (b2 * (2 * x + 1) < a2 * (2 * y - 1)) {
        if (sigma >= 0) {
            sigma += fa2 * (1 - y);
            y--;
        }
        sigma += b2 * ((4 * x) + 6);
        x++;
        drawEllipsePoints(x, y, m_colorBorder);
    }

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

std::string EllipseShape::Serialize() const {
    std::ostringstream oss;
    oss << "FILLED_ELLIPSE " << m_bounds.x << " " << m_bounds.y << " " << m_bounds.width << " " << m_bounds.height << " "
        << (m_colorBorder.r / 255.f) << " " << (m_colorBorder.g / 255.f) << " " << (m_colorBorder.b / 255.f) << " "
        << (m_color.r / 255.f) << " " << (m_color.g / 255.f) << " " << (m_color.b / 255.f);
    return oss.str();
}

bool EllipseShape::TryGrabControlPoint(Vector2 point) {
    float hitbox = 8.0f;
    Vector2 pts[4] = {
        {m_bounds.x + m_bounds.width/2, m_bounds.y},
        {m_bounds.x + m_bounds.width/2, m_bounds.y + m_bounds.height},
        {m_bounds.x, m_bounds.y + m_bounds.height/2},
        {m_bounds.x + m_bounds.width, m_bounds.y + m_bounds.height/2}
    };
    for(int i=0; i<4; i++) {
        if (CheckCollisionPointCircle(point, pts[i], hitbox)) {
            m_draggingControlPoint = i;
            return true;
        }
    }
    return false;
}

void EllipseShape::DragControlPoint(Vector2 point) {
    if (m_draggingControlPoint == 0) { 
        float diff = point.y - m_bounds.y; m_bounds.y = point.y; m_bounds.height -= diff;
    } else if (m_draggingControlPoint == 1) { 
        m_bounds.height = point.y - m_bounds.y;
    } else if (m_draggingControlPoint == 2) { 
        float diff = point.x - m_bounds.x; m_bounds.x = point.x; m_bounds.width -= diff;
    } else if (m_draggingControlPoint == 3) { 
        m_bounds.width = point.x - m_bounds.x;
    }
    if (m_bounds.width < 1) m_bounds.width = 1;
    if (m_bounds.height < 1) m_bounds.height = 1;
}

void EllipseShape::StopDragging() { m_draggingControlPoint = -1; }