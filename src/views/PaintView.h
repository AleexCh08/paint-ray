#pragma once
#include "../models/PaintDocument.h"
#include "raylib.h"

class PaintView {
private:
    PaintDocument* m_document;
    Font m_font;
    Font m_iconFont;

public:
    PaintView(PaintDocument* document);
    ~PaintView(); 
    
    void Render();
};