#include "PaintDocument.h"
#include <algorithm>

PaintDocument::PaintDocument() {
    m_selectedShape = nullptr;
    m_currentMode = MODE_OPTIMIZED;
    m_currentTool = SHAPE_NONE;
    m_currentFillColor = WHITE;     
    m_currentBorderColor = BLACK;   
    m_editBorderMode = false;
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
void PaintDocument::SetCurrentFillColor(Color color) { m_currentFillColor = color; }
Color PaintDocument::GetCurrentFillColor() const { return m_currentFillColor; }
void PaintDocument::SetCurrentBorderColor(Color color) { m_currentBorderColor = color; }
Color PaintDocument::GetCurrentBorderColor() const { return m_currentBorderColor; }
void PaintDocument::SetEditBorderMode(bool editBorder) { m_editBorderMode = editBorder; }
bool PaintDocument::IsEditBorderMode() const { return m_editBorderMode; }