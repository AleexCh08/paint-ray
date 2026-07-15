#include "PaintDocument.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cmath>
#include "../shapes/Line.h"
#include "../shapes/RectangleShape.h"
#include "../shapes/EllipseShape.h"
#include "../shapes/Triangle.h"
#include "../shapes/Circle.h"
#include "../shapes/BezierCurve.h" 

PaintDocument::PaintDocument() {
    m_selectedShape = nullptr;
    m_currentMode = MODE_OPTIMIZED;
    m_currentTool = SHAPE_NONE;
    m_currentFillColor = WHITE;     
    m_currentBorderColor = BLACK;   
    m_editBorderMode = false;
    m_showContextMenu = false;
    m_contextMenuPos = {0, 0};
    m_bgColor = { 210, 210, 210, 255 };
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

void PaintDocument::MoveShapeToFront(Shape* shape) {
    auto it = std::find(m_shapes.begin(), m_shapes.end(), shape);
    if (it != m_shapes.end()) {
        m_shapes.erase(it);
        m_shapes.push_back(shape); 
    }
}

void PaintDocument::MoveShapeToBack(Shape* shape) {
    auto it = std::find(m_shapes.begin(), m_shapes.end(), shape);
    if (it != m_shapes.end()) {
        m_shapes.erase(it);
        m_shapes.insert(m_shapes.begin(), shape); 
    }
}

void PaintDocument::MoveShapeForward(Shape* shape) {
    auto it = std::find(m_shapes.begin(), m_shapes.end(), shape);
    if (it != m_shapes.end() && it != m_shapes.end() - 1) {
        std::iter_swap(it, it + 1);
    }
}

void PaintDocument::MoveShapeBackward(Shape* shape) {
    auto it = std::find(m_shapes.begin(), m_shapes.end(), shape);
    if (it != m_shapes.end() && it != m_shapes.begin()) {
        std::iter_swap(it, it - 1);
    }
}

void PaintDocument::SetContextMenuState(bool open, Vector2 pos) {
    m_showContextMenu = open;
    m_contextMenuPos = pos;
}

bool PaintDocument::IsContextMenuOpen() const { return m_showContextMenu; }
Vector2 PaintDocument::GetContextMenuPos() const { return m_contextMenuPos; }

void PaintDocument::SaveToFile(const std::string& filename) const {
    std::ofstream out(filename);
    if (!out) return;
    
    out << m_shapes.size() << "\n";
    out << "BACKGROUND " << (m_bgColor.r / 255.f) << " " << (m_bgColor.g / 255.f) << " " << (m_bgColor.b / 255.f) << "\n";
    
    for (Shape* shape : m_shapes) {
        out << shape->Serialize() << "\n";
    }
}

void PaintDocument::LoadFromFile(const std::string& filename) {
    std::ifstream in(filename);
    if (!in) return;

    ClearAll();
    int count;
    if (!(in >> count)) return;

    std::string token;
    if (in >> token && token == "BACKGROUND") {
        float r, g, b;
        in >> r >> g >> b;
        m_bgColor = { (unsigned char)(r * 255), (unsigned char)(g * 255), (unsigned char)(b * 255), 255 };
    }

    for (int i = 0; i < count; ++i) {
        if (!(in >> token)) break;

        if (token == "LINE") {
            float x1, y1, x2, y2, r, g, b;
            in >> x1 >> y1 >> x2 >> y2 >> r >> g >> b;
            Color cb = { (unsigned char)(r*255), (unsigned char)(g*255), (unsigned char)(b*255), 255 };
            AddShape(new Line({x1, y1}, {x2, y2}, cb));
        }
        else if (token == "BEZIER") {
            int n; float x1, y1, x2, y2, x3, y3, x4, y4, r, g, b;
            in >> n >> x1 >> y1 >> x2 >> y2 >> x3 >> y3 >> x4 >> y4 >> r >> g >> b;
            Color cb = { (unsigned char)(r*255), (unsigned char)(g*255), (unsigned char)(b*255), 255 };
            BezierCurve* bz = new BezierCurve({x1, y1}, {x4, y4}, cb);
            bz->SetControlPoints({x1, y1}, {x2, y2}, {x3, y3}, {x4, y4});
            AddShape(bz);
        }
        else if (token == "RECTANGLE" || token == "FILLED_RECTANGLE") {
            float x, y, w, h, br, bg, bb;
            in >> x >> y >> w >> h >> br >> bg >> bb;
            Color cb = { (unsigned char)(br*255), (unsigned char)(bg*255), (unsigned char)(bb*255), 255 };
            Color cf = { 0, 0, 0, 0 }; // Transparente si no es FILLED
            if (token == "FILLED_RECTANGLE") {
                float fr, fg, fb;
                in >> fr >> fg >> fb;
                cf = { (unsigned char)(fr*255), (unsigned char)(fg*255), (unsigned char)(fb*255), 255 };
            }
            RectangleShape* r = new RectangleShape({x, y, 0, 0}, cf);
            r->SetBounds({x, y, w, h});
            r->SetColorBorder(cb);
            AddShape(r);
        }
        else if (token == "ELLIPSE" || token == "FILLED_ELLIPSE") {
            float x, y, w, h, br, bg, bb;
            in >> x >> y >> w >> h >> br >> bg >> bb;
            Color cb = { (unsigned char)(br*255), (unsigned char)(bg*255), (unsigned char)(bb*255), 255 };
            Color cf = { 0, 0, 0, 0 };
            if (token == "FILLED_ELLIPSE") {
                float fr, fg, fb;
                in >> fr >> fg >> fb;
                cf = { (unsigned char)(fr*255), (unsigned char)(fg*255), (unsigned char)(fb*255), 255 };
            }
            EllipseShape* e = new EllipseShape({x, y, 0, 0}, cf);
            e->SetBounds({x, y, w, h});
            e->SetColorBorder(cb);
            AddShape(e);
        }
        else if (token == "TRIANGLE" || token == "FILLED_TRIANGLE") {
            float x1, y1, x2, y2, x3, y3, br, bg, bb;
            in >> x1 >> y1 >> x2 >> y2 >> x3 >> y3 >> br >> bg >> bb;
            Color cb = { (unsigned char)(br*255), (unsigned char)(bg*255), (unsigned char)(bb*255), 255 };
            Color cf = { 0, 0, 0, 0 };
            if (token == "FILLED_TRIANGLE") {
                float fr, fg, fb;
                in >> fr >> fg >> fb;
                cf = { (unsigned char)(fr*255), (unsigned char)(fg*255), (unsigned char)(fb*255), 255 };
            }
            Triangle* t = new Triangle({x1, y1}, {x2, y2}, {x3, y3}, cf);
            t->SetColorBorder(cb);
            AddShape(t);
        }
        else if (token == "CIRCLE" || token == "FILLED_CIRCLE") {
            float x1, y1, x2, y2, br, bg, bb;
            in >> x1 >> y1 >> x2 >> y2 >> br >> bg >> bb;
            Color cb = { (unsigned char)(br*255), (unsigned char)(bg*255), (unsigned char)(bb*255), 255 };
            Color cf = { 0, 0, 0, 0 };
            if (token == "FILLED_CIRCLE") {
                float fr, fg, fb;
                in >> fr >> fg >> fb;
                cf = { (unsigned char)(fr*255), (unsigned char)(fg*255), (unsigned char)(fb*255), 255 };
            }
            float radius = std::sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
            Circle* c = new Circle({x1, y1}, radius, cf);
            c->SetColorBorder(cb);
            AddShape(c);
        }
    }
}