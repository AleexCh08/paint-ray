#include "PaintController.h"
#include "../shapes/Line.h"
#include "../shapes/Circle.h"
#include "../shapes/RectangleShape.h"
#include "../shapes/EllipseShape.h"
#include "../shapes/Triangle.h"
#include "../shapes/BezierCurve.h"
#include "raymath.h"

PaintController::PaintController(PaintDocument* document) 
    : m_document(document), m_isDrawing(false), m_isMovingShape(false), m_isDraggingPoint(false) {}

void PaintController::Update() {
    if (IsKeyPressed(KEY_ESCAPE)) m_document->SetSelectedShape(nullptr);

    Vector2 mousePos = GetMousePosition();
    
    Rectangle canvas = { 280, 60, (float)GetScreenWidth() - 400, (float)GetScreenHeight() - 80 };
    bool isInsideCanvas = CheckCollisionPointRec(mousePos, canvas);

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && isInsideCanvas) HandleMouseDown(mousePos);
    else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) HandleMouseDrag(mousePos);
    else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) HandleMouseUp();

    if (IsKeyPressed(KEY_DELETE)) {
        Shape* selected = m_document->GetSelectedShape();
        if (selected) m_document->RemoveShape(selected);
    }
}

Shape* PaintController::GetShapeAtPoint(Vector2 point) {
    const auto& shapes = m_document->GetShapes();
    // Iteramos en reversa para seleccionar primero la figura que está por encima
    for (auto it = shapes.rbegin(); it != shapes.rend(); ++it) {
        if ((*it)->IsPointInside(point)) {
            return *it;
        }
    }
    return nullptr;
}

void PaintController::HandleMouseDown(Vector2 mousePos) {
    m_lastMousePos = mousePos;
    Shape* clickedShape = GetShapeAtPoint(mousePos);

    if (clickedShape) {
        m_document->SetSelectedShape(clickedShape);
        m_isMovingShape = true;
        m_isDraggingPoint = false;
        
        // Verificamos explícitamente si se agarró un punto de control
        if (clickedShape->m_type == SHAPE_LINE) {
            m_isDraggingPoint = ((Line*)clickedShape)->TryGrabControlPoint(mousePos);
        } else if (clickedShape->m_type == SHAPE_BEZIER) {
            m_isDraggingPoint = ((BezierCurve*)clickedShape)->TryGrabControlPoint(mousePos);
        }
        return;
    }

    m_document->SetSelectedShape(nullptr);
    if (m_document->GetCurrentTool() == SHAPE_NONE) return;

    m_isDrawing = true;
    Shape* newShape = nullptr;
    Color fillColor = m_document->GetCurrentFillColor();
    Color borderColor = m_document->GetCurrentBorderColor();

    switch (m_document->GetCurrentTool()) {
        case SHAPE_LINE:      newShape = new Line(mousePos, mousePos, borderColor); break;
        case SHAPE_BEZIER:    newShape = new BezierCurve(mousePos, mousePos, borderColor); break;
        case SHAPE_CIRCLE:    newShape = new Circle(mousePos, 0, fillColor); break;
        case SHAPE_RECTANGLE: newShape = new RectangleShape({mousePos.x, mousePos.y, 0, 0}, fillColor); break;
        case SHAPE_ELLIPSE:   newShape = new EllipseShape({mousePos.x, mousePos.y, 0, 0}, fillColor); break;
        case SHAPE_TRIANGLE:  newShape = new Triangle(mousePos, mousePos, mousePos, fillColor); break;
        default: break;
    }

    if (newShape && m_document->GetCurrentTool() != SHAPE_LINE && m_document->GetCurrentTool() != SHAPE_BEZIER) {
        newShape->SetColorBorder(borderColor);
    }

    if (newShape) {
        m_document->AddShape(newShape);
        m_document->SetSelectedShape(newShape);
    }
}

void PaintController::HandleMouseDrag(Vector2 mousePos) {
    Shape* selected = m_document->GetSelectedShape();
    if (!selected) return;

    if (m_isDrawing) {
        switch (selected->m_type) {
            case SHAPE_LINE: ((Line*)selected)->SetEnd(mousePos); break;
            case SHAPE_CIRCLE: ((Circle*)selected)->SetRadius(Vector2Distance(m_lastMousePos, mousePos)); break;
            case SHAPE_RECTANGLE: {
                float rx = fminf(m_lastMousePos.x, mousePos.x), ry = fminf(m_lastMousePos.y, mousePos.y);
                float rw = fabsf(mousePos.x - m_lastMousePos.x), rh = fabsf(mousePos.y - m_lastMousePos.y);
                ((RectangleShape*)selected)->SetBounds({rx, ry, rw, rh});
                break;
            }
            case SHAPE_ELLIPSE: {
                float ex = fminf(m_lastMousePos.x, mousePos.x), ey = fminf(m_lastMousePos.y, mousePos.y);
                float ew = fabsf(mousePos.x - m_lastMousePos.x), eh = fabsf(mousePos.y - m_lastMousePos.y);
                ((EllipseShape*)selected)->SetBounds({ex, ey, ew, eh});
                break;
            }
            case SHAPE_TRIANGLE: ((Triangle*)selected)->SetPoints(m_lastMousePos, {mousePos.x, m_lastMousePos.y}, {m_lastMousePos.x, mousePos.y}); break;
            case SHAPE_BEZIER: ((BezierCurve*)selected)->SetPoints(m_lastMousePos, mousePos); break;
            default: break;
        }
    } 
    else if (m_isMovingShape) {
        // Si agarramos un punto, lo arrastramos. Si no, movemos la figura entera.
        if (m_isDraggingPoint) {
            if (selected->m_type == SHAPE_LINE) ((Line*)selected)->DragControlPoint(mousePos);
            else if (selected->m_type == SHAPE_BEZIER) ((BezierCurve*)selected)->DragControlPoint(mousePos);
        } else {
            Vector2 offset = { mousePos.x - m_lastMousePos.x, mousePos.y - m_lastMousePos.y };
            selected->Move(offset);
        }
        m_lastMousePos = mousePos;
    }
}

void PaintController::HandleMouseUp() {
    Shape* selected = m_document->GetSelectedShape();
    if (selected) {
        if (selected->m_type == SHAPE_LINE) ((Line*)selected)->StopDragging();
        if (selected->m_type == SHAPE_BEZIER) ((BezierCurve*)selected)->StopDragging();
    }

    m_isDrawing = false;
    m_isMovingShape = false;
}