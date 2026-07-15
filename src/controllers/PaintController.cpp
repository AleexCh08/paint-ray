#include "PaintController.h"
#include "../shapes/Line.h"
#include "../shapes/Circle.h"
#include "../shapes/RectangleShape.h"
#include "../shapes/EllipseShape.h"
#include "../shapes/Triangle.h"
#include "../shapes/BezierCurve.h"
#include "raymath.h"

PaintController::PaintController(PaintDocument* document) 
    : m_document(document), m_isDrawing(false), m_isMovingShape(false) {}

void PaintController::Update() {
    Vector2 mousePos = GetMousePosition();
    bool isMouseOverUI = (mousePos.y <= 40) || (mousePos.x <= 80);
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !isMouseOverUI) {
        HandleMouseDown(mousePos);
    } 
    else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        HandleMouseDrag(mousePos);
    } 
    else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        HandleMouseUp();
    }

    // Tecla Suprimir para borrar
    if (IsKeyPressed(KEY_DELETE)) {
        Shape* selected = m_document->GetSelectedShape();
        if (selected) {
            m_document->RemoveShape(selected);
        }
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

    // 1. Si hicimos clic en una figura, la seleccionamos
    if (clickedShape) {
        m_document->SetSelectedShape(clickedShape);
        m_isMovingShape = true;
        
        // Comprobar si es una línea o curva para arrastrar puntos de control
        if (clickedShape->m_type == SHAPE_LINE) {
            ((Line*)clickedShape)->HandleDragging(mousePos);
        } else if (clickedShape->m_type == SHAPE_BEZIER) {
            ((BezierCurve*)clickedShape)->HandleDragging(mousePos);
        }
        return;
    }

    // 2. Si hicimos clic en el vacío, empezamos a dibujar una nueva figura
    m_document->SetSelectedShape(nullptr);
    m_isDrawing = true;
    Shape* newShape = nullptr;
    Color color = m_document->GetCurrentColor();

    switch (m_document->GetCurrentTool()) {
        case SHAPE_LINE:      newShape = new Line(mousePos, mousePos, color); break;
        case SHAPE_CIRCLE:    newShape = new Circle(mousePos, 0, color); break;
        case SHAPE_RECTANGLE: newShape = new RectangleShape({mousePos.x, mousePos.y, 0, 0}, color); break;
        case SHAPE_ELLIPSE:   newShape = new EllipseShape({mousePos.x, mousePos.y, 0, 0}, color); break;
        case SHAPE_TRIANGLE:  newShape = new Triangle(mousePos, mousePos, mousePos, color); break;
        case SHAPE_BEZIER:    newShape = new BezierCurve(mousePos, mousePos, color); break;
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
            case SHAPE_LINE: 
                ((Line*)selected)->SetEnd(mousePos); 
                break;
            case SHAPE_CIRCLE: {
                Circle* c = (Circle*)selected;
                Vector2 center = { (float)c->GetColorBorder().r, 0 }; 
                c->SetRadius(Vector2Distance(m_lastMousePos, mousePos));
                break;
            }
            case SHAPE_RECTANGLE: {
                RectangleShape* r = (RectangleShape*)selected;
                float rx = fminf(m_lastMousePos.x, mousePos.x);
                float ry = fminf(m_lastMousePos.y, mousePos.y);
                float rw = fabsf(mousePos.x - m_lastMousePos.x);
                float rh = fabsf(mousePos.y - m_lastMousePos.y);
                r->SetBounds({rx, ry, rw, rh});
                break;
            }
            case SHAPE_ELLIPSE: {
                EllipseShape* e = (EllipseShape*)selected;
                float ex = fminf(m_lastMousePos.x, mousePos.x);
                float ey = fminf(m_lastMousePos.y, mousePos.y);
                float ew = fabsf(mousePos.x - m_lastMousePos.x);
                float eh = fabsf(mousePos.y - m_lastMousePos.y);
                e->SetBounds({ex, ey, ew, eh});
                break;
            }
            case SHAPE_TRIANGLE: {
                Triangle* t = (Triangle*)selected;
                t->SetPoints(m_lastMousePos, {mousePos.x, m_lastMousePos.y}, {m_lastMousePos.x, mousePos.y});
                break;
            }
            case SHAPE_BEZIER:
                ((BezierCurve*)selected)->SetPoints(m_lastMousePos, mousePos);
                break;
        }
    } 
    else if (m_isMovingShape) {
        // Lógica de arrastrar puntos de control o mover la figura entera
        if (selected->m_type == SHAPE_LINE) {
            ((Line*)selected)->HandleDragging(mousePos);
        } else if (selected->m_type == SHAPE_BEZIER) {
            ((BezierCurve*)selected)->HandleDragging(mousePos);
        } else {
            Vector2 offset = { mousePos.x - m_lastMousePos.x, mousePos.y - m_lastMousePos.y };
            selected->Move(offset);
        }
        m_lastMousePos = mousePos; // Actualizamos para el próximo frame
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