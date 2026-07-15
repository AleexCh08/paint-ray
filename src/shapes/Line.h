#pragma once
#include "Shape.h"

class Line : public Shape {
private:
    Vector2 m_start;
    Vector2 m_end;
    Color m_color;
    Color m_colorBorder;
    int m_draggingControlPoint;

public:
    Line(Vector2 start, Vector2 end, Color color);
    ~Line() override;

    void RenderPixelByPixel() override;
    void RenderOptimized() override;
    void DrawSelection() override;

    void Move(Vector2 offset) override;
    bool IsPointInside(Vector2 point) override;

    void SetColor(Color color) override;
    Color GetColor() const override;

    void SetColorBorder(Color color) override;
    Color GetColorBorder() const override;

    // Métodos específicos
    void SetEnd(Vector2 end);
    void HandleDragging(Vector2 point);
    void StopDragging();

private:
    void DrawControlPoints();
    bool IsCursorOnControlPoint(Vector2 point) const;
};