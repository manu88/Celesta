//
//  UISlider.hpp
//  JSCenter
//
//  Created by Manuel Deneu on 24/11/2015.
//  Copyright © 2015 Manuel Deneu. All rights reserved.
//

#ifndef UISlider_hpp
#define UISlider_hpp

#include <stdio.h>

#include "../GX/GXAsyncPainter.h"
#include "../GX/GXTextRenderer.h"

#include "UIView.h"

class UISlider : public UIView
{
public:
    
    static GXSize  DefaultSize;
    
    UISlider();
    ~UISlider();
    
    
    void setPosition( const GXPoint &pt);
    
    void setValue( float val);
    
    float getValue() const noexcept
    {
        return _value;
    }
    
    void resetState()
    {
        setValue(0.5);
    }
    
#ifdef ENABLE_ELEMENT_SELECTOR
    const Variant performSelectorWithArguments( const std::string &selector , const VariantList  &arguments , bool *validCall = nullptr) ;
#endif
    
#ifdef USE_JSON_PARSER
    static UISlider* createFromJSON( const JSON::Node &node );
#endif

protected:
    
#ifdef USE_JSON_PARSER
    bool saveJSONSpec( JSON::Node &node) const;
#endif
    
private:
    bool toucheBegan( const std::list<GXTouch> &touches );
    bool toucheMoved( const std::list<GXTouch> &touches );
    bool toucheEnded( const std::list<GXTouch> &touches );
    
    GXAsyncPainter *_painter;
    GXAsyncPainter *_cursor;
    
    float _value;
};

#endif /* UISlider_hpp */
