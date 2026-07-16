#pragma once
#include "../models/PaintDocument.h"
#include "raylib.h"

class PaintView {
private:
    PaintDocument* m_document;
    Font m_font;

public:
    PaintView(PaintDocument* document);
    ~PaintView(); 
    
    void Render();
};