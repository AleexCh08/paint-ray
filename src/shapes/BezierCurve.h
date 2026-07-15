#pragma once
#include "Shape.h"

class BezierCurve : public Shape {
private:
    Vector2 m_points[4]; // 0: Inicio, 1: Control 1, 2: Control 2, 3: Fin
    Color m_color;
    Color m_colorBorder;
    int m_draggingControlPoint;

public:
    BezierCurve(Vector2 start, Vector2 end, Color color);
    ~BezierCurve() override;

    void RenderPixelByPixel() override;
    void RenderOptimized() override;
    void DrawSelection() override;

    void Move(Vector2 offset) override;
    bool IsPointInside(Vector2 point) override;

    void SetColor(Color color) override;
    Color GetColor() const override;

    void SetColorBorder(Color color) override;
    Color GetColorBorder() const override;

    // Métodos específicos de la curva
    void SetPoints(Vector2 start, Vector2 end);
    bool TryGrabControlPoint(Vector2 point);
    void DragControlPoint(Vector2 point);
    void StopDragging();

private:
    Vector2 CalculateBezierPoint(float t) const;
    void DrawControlPoints();
};