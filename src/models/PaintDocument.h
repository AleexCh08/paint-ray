#pragma once
#include <vector>
#include "../shapes/Shape.h"

class PaintDocument {
private:
    std::vector<Shape*> m_shapes;
    Shape* m_selectedShape;
    
    DrawingMode m_currentMode;
    ShapeType m_currentTool;
    Color m_currentFillColor;
    Color m_currentBorderColor;
    bool m_editBorderMode;

public:
    PaintDocument();
    ~PaintDocument();

    // Gestión de figuras
    void AddShape(Shape* shape);
    void RemoveShape(Shape* shape);
    void ClearAll();
    const std::vector<Shape*>& GetShapes() const;

    // Estado de selección
    void SetSelectedShape(Shape* shape);
    Shape* GetSelectedShape() const;

    // Estados generales
    void SetDrawingMode(DrawingMode mode);
    DrawingMode GetDrawingMode() const;

    void SetCurrentTool(ShapeType tool);
    ShapeType GetCurrentTool() const;

    void SetCurrentFillColor(Color color);
    Color GetCurrentFillColor() const;

    void SetCurrentBorderColor(Color color);
    Color GetCurrentBorderColor() const;

    void SetEditBorderMode(bool editBorder);
    bool IsEditBorderMode() const;
};