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
    if (IsKeyPressed(KEY_ESCAPE)) {
        m_document->SetSelectedShape(nullptr);
        m_document->SetContextMenuState(false);
    }

    Vector2 mousePos = GetMousePosition();
    Rectangle canvas = { 280, 60, (float)GetScreenWidth() - 400, (float)GetScreenHeight() - 80 };
    bool isInsideCanvas = CheckCollisionPointRec(mousePos, canvas);

    bool isInsideContextMenu = false;
    if (m_document->IsContextMenuOpen()) {
        Vector2 menuPos = m_document->GetContextMenuPos();
        Rectangle menuRect = { menuPos.x, menuPos.y, 140, 184 };
        isInsideContextMenu = CheckCollisionPointRec(mousePos, menuRect);
    }

    int cursorToSet = MOUSE_CURSOR_DEFAULT;
    if (isInsideCanvas && !isInsideContextMenu) {
        if (m_isDrawing || m_isDraggingPoint) {
            cursorToSet = MOUSE_CURSOR_CROSSHAIR; 
        } else if (m_isMovingShape) {
            cursorToSet = MOUSE_CURSOR_POINTING_HAND; 
        } else {
            Shape* hoveredShape = GetShapeAtPoint(mousePos);
            if (m_document->GetCurrentTool() != SHAPE_NONE) {
                cursorToSet = MOUSE_CURSOR_CROSSHAIR; 
            } else if (hoveredShape) {
                cursorToSet = MOUSE_CURSOR_POINTING_HAND; 
            }
        }
    }
    SetMouseCursor(cursorToSet);

    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && isInsideCanvas) {
        Shape* clickedShape = GetShapeAtPoint(mousePos);
        if (clickedShape) {
            m_document->SetSelectedShape(clickedShape);
            m_document->SetContextMenuState(true, mousePos);
        } else {
            m_document->SetContextMenuState(false);
        }
    }
    else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (!isInsideContextMenu) {
            m_document->SetContextMenuState(false);
            if (isInsideCanvas) HandleMouseDown(mousePos);
        }
    }
    else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) HandleMouseDrag(mousePos);
    else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) HandleMouseUp();

    if (IsKeyPressed(KEY_DELETE)) {
        Shape* selected = m_document->GetSelectedShape();
        if (selected) {
            m_document->RemoveShape(selected);
            m_document->SetContextMenuState(false);
        }
    }
}

Shape* PaintController::GetShapeAtPoint(Vector2 point) {
    const auto& shapes = m_document->GetShapes();
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
        m_isDraggingPoint = clickedShape->TryGrabControlPoint(mousePos);
    
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
            selected->DragControlPoint(mousePos);
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
        selected->StopDragging();
    }

    m_isDrawing = false;
    m_isMovingShape = false;
}