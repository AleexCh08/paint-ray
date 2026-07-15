// src/Shape.h
#pragma once
#include "raylib.h"

enum ShapeType {
    SHAPE_NONE = -1,
    SHAPE_LINE = 0,
    SHAPE_CIRCLE,
    SHAPE_ELLIPSE,
    SHAPE_RECTANGLE,
    SHAPE_TRIANGLE,
    SHAPE_BEZIER
};

enum DrawingMode {
    MODE_OPTIMIZED,    //  WIN32_PRIMITIVES
    MODE_PIXEL_BY_PIXEL // CG1_PRIMITIVES
};

class Shape {
public:
    ShapeType m_type;

    virtual ~Shape() {}
    virtual void RenderPixelByPixel() = 0;
    virtual void RenderOptimized() = 0;
    
    virtual void DrawSelection() = 0;

    virtual void Move(Vector2 offset) = 0;
    virtual bool IsPointInside(Vector2 point) = 0;

    virtual void SetColor(Color color) = 0;
    virtual Color GetColor() const = 0;
    
    virtual void SetColorBorder(Color color) = 0;
    virtual Color GetColorBorder() const = 0;
};