//
//  UIButton.hpp
//  JSCenter
//
//  Created by Manuel Deneu on 23/11/2015.
//  Copyright © 2015 Manuel Deneu. All rights reserved.
//

#ifndef UIButton_hpp
#define UIButton_hpp

#include "../GX/GXAsyncPainter.h"
#include "../GX/GXTextRenderer.h"

#include "UIView.h"

class UIButton : public UIView
{
public:
    
    typedef enum
    {
        Momentary = 0, //default
        Toggle    = 1
    } Type;
    
    typedef enum
    {
        Disabled = -1,
        Normal   = 0, // default
        Selected = 1
        
    } State;
    
    /* *** */
    
    static GXSize  DefaultSize;
    
    static GXColor NormalBackgroundColor;
    static GXColor SelectedBackgroundColor;
    
    static GXColor NormalTextColor;
    static GXColor SelectedTextColor;
    
    /* *** */
    
    UIButton();
    ~UIButton();
    
    void setType( UIButton::Type type );
    
    UIButton::Type getType() const noexcept
    {
        return _type;
    }
    
    void setText( const std::string &text);
    void setTextForState( const std::string &text , const State &forState );
    
    const std::string &getText( const State &forState) const
    {
        if( forState == Normal )
            return  _textNormal;
        
        else if( forState == Selected)
            return _textSelected;

        return _textDisabled;
    }
    
    void setPosition( const GXPoint &pt);
    
    const State &getCurrentState() const noexcept
    {
        return _currentState;
    }
    
    void changeStateTo( const State &newState);
    
    void resetState()
    {
        changeStateTo( Normal );
    }
    
    /* Attributes */
    
    void setAttribute( const std::string &attributeName , const Variant &value);
    
    inline void setFont(  const GXFont*  font)
    {
        _text->setFont( font );
    }
    
    /**/
    
#ifdef ENABLE_ELEMENT_SELECTOR
    const Variant performSelectorWithArguments( const std::string &selector , const VariantList  &arguments , bool *validCall = nullptr) ;
#endif
    

    
protected:


    
private:
    void create();
    bool toucheBegan( const std::list<GXTouch> &touches );
    bool toucheMoved( const std::list<GXTouch> &touches );
    bool toucheEnded( const std::list<GXTouch> &touches );
    
    Type  _type;
    State _currentState;
    
    std::string _textNormal;
    std::string _textSelected;
    std::string _textDisabled;
    
    GXAsyncPainter *_painter;
    GXTextRenderer *_text;
    
    GXColor _backgroundColorDisabled;
    GXColor _backgroundColorNormal;
    GXColor _backgroundColorSelected;
    
    GXColor _textColorDisabled;
    GXColor _textColorNormal;
    GXColor _textColorSelected;
};


#endif /* UIButton_hpp */
