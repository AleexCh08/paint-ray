#pragma once
#include "Shape.h"

class Triangle : public Shape {
private:
    Vector2 m_points[3];
    Color m_color;
    Color m_colorBorder;

public:
    Triangle(Vector2 p1, Vector2 p2, Vector2 p3, Color color);
    ~Triangle() override;

    void RenderPixelByPixel() override;
    void RenderOptimized() override;
    void DrawSelection() override;

    void Move(Vector2 offset) override;
    bool IsPointInside(Vector2 point) override;

    void SetColor(Color color) override;
    Color GetColor() const override;

    void SetColorBorder(Color color) override;
    Color GetColorBorder() const override;

    void SetPoints(Vector2 p1, Vector2 p2, Vector2 p3);
    std::string Serialize() const override;
};