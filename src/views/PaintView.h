#pragma once
#include "../models/PaintDocument.h"

class PaintView {
private:
    PaintDocument* m_document;

public:
    PaintView(PaintDocument* document);
    
    // Función que se llamará en cada frame
    void Render();
};