//
//  GXJSON.h
//  Broadway_core
//
//  Created by Manuel Deneu on 27/01/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#ifndef Broadway_core_GXJSON_h
#define Broadway_core_GXJSON_h


#include "../Internal/Element.h"


namespace GXElementAttributes
{
    /* All */
    static const std::string Class            = "Class";
    static const std::string Name             = ElementAttributes::Name;
    static const std::string Layer            = "Layer";
    static const std::string Transparent      = "Transparent";
    static const std::string Bounds           = "Bounds";
    static const std::string Hidden           = "Hidden";
    static const std::string BackgroundColor  = "BackgroundColor";
    
    /* GXImage */
    static const std::string Ressource  = "Ressource";
    
    /* GXText */
    static const std::string Font       = "Font";
    static const std::string FontSize   = "FontSize";
    static const std::string Text       = "Text";
    static const std::string TextColor  = "TextColor";
}

namespace PainterTypeAttribue
{
// basic shapes
    static const std::string Rect        = "Rect";
    static const std::string RoundedRect = "RoundedRect";
    static const std::string Ellipse     = "Ellipse";
    static const std::string Line        = "Line";
    static const std::string Arc         = "Arc";

// path related
    static const std::string MoveTo       = "MoveTo";
    static const std::string LineTo       = "LineTo";
    static const std::string CubicCurveTo = "CubicCurveTo";
    static const std::string QuadCurveTo  = "QuadCurveTo";
    static const std::string Close        = "Close";

// actions
    static const std::string ColorStroke = "ColorStroke";
    static const std::string ColorFill   = "ColorFill";
    static const std::string StrokeWidth = "StrokeWidth";
    static const std::string CapStyle    = "CapStyle";
    static const std::string JoinStyle   = "JoinStyle";
    static const std::string Fill        = "Fill";
    static const std::string Stroke      = "Stroke";
}

#endif /*Broadway_core_GXJSON_h*/
