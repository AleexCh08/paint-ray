#pragma once
#include "../models/PaintDocument.h"

class PaintController {
private:
    PaintDocument* m_document;
    bool m_isDrawing;
    bool m_isMovingShape;
    bool m_isDraggingPoint;
    Vector2 m_lastMousePos;

public:
    PaintController(PaintDocument* document);
    
    // Función que se llamará en cada frame para procesar inputs
    void Update();

private:
    void HandleMouseDown(Vector2 mousePos);
    void HandleMouseDrag(Vector2 mousePos);
    void HandleMouseUp();
    
    Shape* GetShapeAtPoint(Vector2 point);
};