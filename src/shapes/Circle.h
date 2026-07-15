#pragma once
#include "Shape.h"

class Circle : public Shape {
private:
    Vector2 m_center;
    float m_radius;
    Color m_color;
    Color m_colorBorder;

public:
    Circle(Vector2 center, float radius, Color color);
    ~Circle() override;

    void RenderPixelByPixel() override;
    void RenderOptimized() override;
    void DrawSelection() override;

    void Move(Vector2 offset) override;
    bool IsPointInside(Vector2 point) override;

    void SetColor(Color color) override;
    Color GetColor() const override;

    void SetColorBorder(Color color) override;
    Color GetColorBorder() const override;

    void SetRadius(float radius);
};