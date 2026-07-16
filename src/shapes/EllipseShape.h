#pragma once
#include "Shape.h"

class EllipseShape : public Shape {
private:
    Rectangle m_bounds;
    Color m_color;
    Color m_colorBorder;
    int m_draggingControlPoint = -1;

public:
    EllipseShape(Rectangle bounds, Color color);
    ~EllipseShape() override;

    void RenderPixelByPixel() override;
    void RenderOptimized() override;
    void DrawSelection() override;

    void Move(Vector2 offset) override;
    bool IsPointInside(Vector2 point) override;

    void SetColor(Color color) override;
    Color GetColor() const override;

    void SetColorBorder(Color color) override;
    Color GetColorBorder() const override;

    void SetBounds(Rectangle bounds);
    std::string Serialize() const override;

    bool TryGrabControlPoint(Vector2 point) override;
    void DragControlPoint(Vector2 point) override;
    void StopDragging() override;
};