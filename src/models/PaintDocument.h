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
    bool m_showContextMenu;
    Vector2 m_contextMenuPos;
    Color m_bgColor;

public:
    PaintDocument();
    ~PaintDocument();

    void AddShape(Shape* shape);
    void RemoveShape(Shape* shape);
    void ClearAll();
    const std::vector<Shape*>& GetShapes() const;

    void SetSelectedShape(Shape* shape);
    Shape* GetSelectedShape() const;

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

    void MoveShapeToFront(Shape* shape);
    void MoveShapeToBack(Shape* shape);
    void MoveShapeForward(Shape* shape);
    void MoveShapeBackward(Shape* shape);
    void SetContextMenuState(bool open, Vector2 pos = {0, 0});
    bool IsContextMenuOpen() const;
    Vector2 GetContextMenuPos() const;

    Color GetBackgroundColor() const { return m_bgColor; }
    void SaveToFile(const std::string& filename) const;
    void LoadFromFile(const std::string& filename);
};