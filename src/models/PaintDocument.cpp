#include "PaintDocument.h"
#include <algorithm>

PaintDocument::PaintDocument() {
    m_selectedShape = nullptr;
    m_currentMode = MODE_OPTIMIZED;
    m_currentTool = SHAPE_NONE;
    m_currentColor = WHITE;
}

PaintDocument::~PaintDocument() {
    ClearAll();
}

void PaintDocument::AddShape(Shape* shape) {
    m_shapes.push_back(shape);
}

void PaintDocument::RemoveShape(Shape* shape) {
    auto it = std::find(m_shapes.begin(), m_shapes.end(), shape);
    if (it != m_shapes.end()) {
        delete *it;
        m_shapes.erase(it);
        if (m_selectedShape == shape) m_selectedShape = nullptr;
    }
}

void PaintDocument::ClearAll() {
    for (Shape* shape : m_shapes) {
        delete shape;
    }
    m_shapes.clear();
    m_selectedShape = nullptr;
}

const std::vector<Shape*>& PaintDocument::GetShapes() const { return m_shapes; }
void PaintDocument::SetSelectedShape(Shape* shape) { m_selectedShape = shape; }
Shape* PaintDocument::GetSelectedShape() const { return m_selectedShape; }
void PaintDocument::SetDrawingMode(DrawingMode mode) { m_currentMode = mode; }
DrawingMode PaintDocument::GetDrawingMode() const { return m_currentMode; }
void PaintDocument::SetCurrentTool(ShapeType tool) { m_currentTool = tool; }
ShapeType PaintDocument::GetCurrentTool() const { return m_currentTool; }
void PaintDocument::SetCurrentColor(Color color) { m_currentColor = color; }
Color PaintDocument::GetCurrentColor() const { return m_currentColor; }