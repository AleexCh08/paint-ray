#include "BezierCurve.h"
#include <cmath>
#include <sstream>

BezierCurve::BezierCurve(Vector2 start, Vector2 end, Color color) 
    : m_color(color), m_colorBorder(color), m_draggingControlPoint(-1) 
{
    m_type = SHAPE_BEZIER;
    SetPoints(start, end);
}

BezierCurve::~BezierCurve() {}

Vector2 BezierCurve::CalculateBezierPoint(float t) const {
    float u = 1.0f - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;

    Vector2 p;
    p.x = uuu * m_points[0].x + 3 * uu * t * m_points[1].x + 3 * u * tt * m_points[2].x + ttt * m_points[3].x;
    p.y = uuu * m_points[0].y + 3 * uu * t * m_points[1].y + 3 * u * tt * m_points[2].y + ttt * m_points[3].y;
    return p;
}

void BezierCurve::RenderPixelByPixel() {
    int segments = 100;
    Vector2 prevPoint = m_points[0];

    auto drawLineBresenham = [&](int x0, int y0, int x1, int y1, Color color) {
        int dx = std::abs(x1 - x0), dy = std::abs(y1 - y0);
        int sx = (x0 < x1) ? 1 : -1, sy = (y0 < y1) ? 1 : -1;
        int err = dx - dy;
        while (true) {
            DrawPixel(x0, y0, color);
            if (x0 == x1 && y0 == y1) break;
            int e2 = 2 * err;
            if (e2 > -dy) { err -= dy; x0 += sx; }
            if (e2 < dx) { err += dx; y0 += sy; }
        }
    };

    for (int i = 1; i <= segments; i++) {
        float t = (float)i / segments;
        Vector2 currentPoint = CalculateBezierPoint(t);
        drawLineBresenham((int)prevPoint.x, (int)prevPoint.y, (int)currentPoint.x, (int)currentPoint.y, m_colorBorder);
        prevPoint = currentPoint;
    }
}

void BezierCurve::RenderOptimized() {
    DrawSplineSegmentBezierCubic(m_points[0], m_points[1], m_points[2], m_points[3], 2.0f, m_colorBorder);
}

void BezierCurve::DrawSelection() {
    DrawLineV(m_points[0], m_points[1], Fade(GRAY, 0.5f));
    DrawLineV(m_points[3], m_points[2], Fade(GRAY, 0.5f));
    
    DrawSplineSegmentBezierCubic(m_points[0], m_points[1], m_points[2], m_points[3], 4.0f, Fade(RED, 0.5f));    
    DrawControlPoints();
}

void BezierCurve::DrawControlPoints() {
    float ctrlSize = 5.0f;
    DrawCircleV(m_points[0], ctrlSize, BLUE);   // Inicio
    DrawCircleV(m_points[1], ctrlSize, GREEN);  // Tensor 1 (Verde para distinguirlo)
    DrawCircleV(m_points[2], ctrlSize, GREEN);  // Tensor 2 (Verde para distinguirlo)
    DrawCircleV(m_points[3], ctrlSize, BLUE);   // Fin
}

void BezierCurve::Move(Vector2 offset) {
    for (int i = 0; i < 4; i++) {
        m_points[i].x += offset.x;
        m_points[i].y += offset.y;
    }
}

bool BezierCurve::IsPointInside(Vector2 point) {
    for (float t = 0.0f; t <= 1.0f; t += 0.02f) {
        Vector2 p = CalculateBezierPoint(t);
        if (CheckCollisionPointCircle(point, p, 5.0f)) {
            return true;
        }
    }
    return false;
}

void BezierCurve::SetColor(Color color) { m_color = color; }
Color BezierCurve::GetColor() const { return m_color; }
void BezierCurve::SetColorBorder(Color color) { m_colorBorder = color; }
Color BezierCurve::GetColorBorder() const { return m_colorBorder; }

void BezierCurve::SetPoints(Vector2 start, Vector2 end) {
    m_points[0] = start;
    m_points[3] = end;
    m_points[1] = { start.x + (end.x - start.x) / 3.0f, start.y + (end.y - start.y) / 3.0f };
    m_points[2] = { start.x + (end.x - start.x) * 2.0f / 3.0f, start.y + (end.y - start.y) * 2.0f / 3.0f };
}

bool BezierCurve::TryGrabControlPoint(Vector2 point) {
    float ctrlSizeHitbox = 8.0f; 
    for (int i = 0; i < 4; i++) {
        if (CheckCollisionPointCircle(point, m_points[i], ctrlSizeHitbox)) {
            m_draggingControlPoint = i;
            return true;
        }
    }
    return false;
}

void BezierCurve::DragControlPoint(Vector2 point) {
    if (m_draggingControlPoint != -1) m_points[m_draggingControlPoint] = point;
}

void BezierCurve::StopDragging() {
    m_draggingControlPoint = -1;
}

void BezierCurve::SetControlPoints(Vector2 p0, Vector2 p1, Vector2 p2, Vector2 p3) {
    m_points[0] = p0; m_points[1] = p1; m_points[2] = p2; m_points[3] = p3;
}

std::string BezierCurve::Serialize() const {
    std::ostringstream oss;
    oss << "BEZIER 4 " << m_points[0].x << " " << m_points[0].y << " " << m_points[1].x << " " << m_points[1].y << " "
        << m_points[2].x << " " << m_points[2].y << " " << m_points[3].x << " " << m_points[3].y << " "
        << (m_colorBorder.r / 255.f) << " " << (m_colorBorder.g / 255.f) << " " << (m_colorBorder.b / 255.f);
    return oss.str();
}