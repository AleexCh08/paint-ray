#pragma once
#include "Shape.h"

class RectangleShape : public Shape {
private:
    Rectangle m_bounds;
    Color m_color;
    Color m_colorBorder;

public:
    RectangleShape(Rectangle bounds, Color color);
    ~RectangleShape() override;

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
};