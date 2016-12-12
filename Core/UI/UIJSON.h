//
//  UIJSON.h
//  JSCenter
//
//  Created by Manuel Deneu on 09/12/2015.
//  Copyright © 2015 Manuel Deneu. All rights reserved.
//

#ifndef UIJSON_h
#define UIJSON_h


#include "../GX/GXJSON.h"
namespace UIAttribute
{
    
    static const std::string Type     = GXElementAttributes::Class;// "Class";
    static const std::string Name     =  GXElementAttributes::Name;// "Name";
    static const std::string Children = "Children";
    static const std::string Position = "Position";
    static const std::string Size     = "Size";
    
    static const std::string Selector = "Selector";
    static const std::string Target   = "Target";
    static const std::string Text     = GXElementAttributes::Text;// "Text";
    static const std::string Value    = "Value";
    static const std::string DefaultValue    = "DefaultValue";

    static const std::string TouchesEnabled    = "TouchesEnabled";
    
    static const std::string ButtonType   = "ButtonType";
    static const std::string Ressource    = GXElementAttributes::Ressource;// "Ressource";
    static const std::string Font         = GXElementAttributes::Font;// "TextFont";
    static const std::string FontSize     = GXElementAttributes::FontSize;// "TextFont";
    /* colors */
    static const std::string BackgroundColor = GXElementAttributes::BackgroundColor;// "BackgroundColor";
    static const std::string TextColor       = GXElementAttributes::TextColor;//"textColor";
    
    
    /* Buttons */
    
    static const std::string NormalText   = "NormalText";
    static const std::string SelectedText = "SelectedText";
    static const std::string DisabledText = "DisabledText";
}

#endif /* UIJSON_h */
