//
//  UIButton.cpp
//  JSCenter
//
//  Created by Manuel Deneu on 23/11/2015.
//  Copyright © 2015 Manuel Deneu. All rights reserved.
//

#include "UIButton.h"

#include "../GX/GXFont.h"
#include "../Env_Variables.h"
#include "UIJSON.h"

GXSize  UIButton::DefaultSize      = makeSize(150, 70);



GXColor UIButton::NormalBackgroundColor     = makeColor(127, 140, 140);
GXColor UIButton::SelectedBackgroundColor   = makeColor(200, 140, 200);

GXColor UIButton::NormalTextColor       = makeColor(220, 220, 220);
GXColor UIButton::SelectedTextColor     = makeColor( 0, 0, 0);




UIButton::UIButton():
_type         ( Momentary ),
_currentState ( Normal ),
_painter ( new GXAsyncPainter() ),
_text    ( new GXTextRenderer() ),

_backgroundColorDisabled ( NormalBackgroundColor ),
_backgroundColorNormal   ( NormalBackgroundColor ),
_backgroundColorSelected ( SelectedBackgroundColor ),
_textColorDisabled       ( NormalTextColor ),
_textColorNormal         ( NormalTextColor ),
_textColorSelected       ( SelectedTextColor )
{
    setElementName( ClassNames::UIButton );
    className = ClassNames::UIButton ;

    setTransparency(true);
    setSize( DefaultSize );


    
    create();
}

UIButton::~UIButton()
{
}

void UIButton::create()
{
    _painter->setLayer(0);
    _painter->setTransparency(true);
    _painter->setSize(getBounds().size);
    
    _painter->clear();
    _painter->setFillColor( _backgroundColorNormal );
    _painter->setStrokeColor( _backgroundColorNormal );
    
    _painter->addRoundedRect( makeRect(makePointNULL(), getBounds().size ), 30, 30);
    
    _painter->fill();
    _painter->stroke();
    
    _text->setFont( GXFont::getFontByName( DefaultsValues::DefaultFont ) );
    _text->setTextColor( NormalTextColor );
    // size 10 pour height 100 ,
    _text->setSizeInPoints( getBounds().size.height/10 );
    
    _text->setLayer(1);
    _text->setTransparency( true );
    _text->setSize( getBounds().size );
    
    
    addElement( _painter);
    addElement( _text );
    
}

void UIButton::setType( UIButton::Type type )
{
    _type = type;

    updateValueForKey(UIAttribute::ButtonType, Variant( _type ));
}

void UIButton::setText( const std::string &text)
{
    setTextForState(text, Normal);
    setTextForState(text, Selected);
    setTextForState(text, Disabled);


}
void UIButton::setTextForState( const std::string &text, const State &forState )
{
    if( forState == Normal )
    {
        if ( _textNormal != text)
        {
            _textNormal = text;
            updateValueForKey(UIAttribute::NormalText, Variant(text));
        }
    }
    else if( forState == Selected )
    {
        if(_textSelected != text )
        {
            _textSelected = text;
            updateValueForKey(UIAttribute::SelectedText, Variant(text));
        }
    }
    else if( forState == Disabled )
    {
        if(_textDisabled != text )
        {
            _textDisabled = text;
            updateValueForKey(UIAttribute::DisabledText, Variant(text));
        }
    }
    

}

void UIButton::setPosition( const GXPoint &pt)
{
    setBounds(makeRect(pt, getBounds().size ) );
  
    _painter->setBounds( getBounds() );
    _text->setBounds( makeRect(
                               makePoint(pt.x, pt.y + getBounds().size.height ),
                               getBounds().size
                               )
                     );

    _painter->setNeedsDisplay();
    setNeedsDisplay();
}

void UIButton::changeStateTo( const State &newState)
{
   //_painter.clear();
    if( newState == Selected )
    {

        _painter->setFillColor  ( _backgroundColorSelected );
        _painter->setStrokeColor( _backgroundColorSelected );
        _text->setDisplayedText( _textSelected );
        
        _text->setTextColor( _textColorSelected );

    }
    
    else if( newState == Normal )
    {
        _painter->setFillColor  ( _backgroundColorNormal );
        _painter->setStrokeColor( _backgroundColorNormal );
        _text->setDisplayedText( _textNormal );
        _text->setTextColor( _textColorNormal );
    }
    else if( newState == Disabled )
    {
        _painter->setFillColor  ( _backgroundColorDisabled );
        _painter->setStrokeColor( _backgroundColorDisabled );
        _text->setDisplayedText( _textDisabled );
        _text->setTextColor( _textColorDisabled );
    }

    _painter->fill();
    _painter->stroke();
    
    _painter->setNeedsDisplay();

    _text->setNeedsDisplay();

    _currentState = newState;
}


void UIButton::setAttribute( const std::string &attributeName , const Variant &value)
{

    if( attributeName == UIAttribute::BackgroundColor)
    {
        _backgroundColorNormal = makeColor( value );
    }
    else if (attributeName == UIAttribute::TextColor )
    {
        _textColorNormal = makeColor( value );
    }
    else if( attributeName == UIAttribute::Font )
    {
        setFont( GXFont::getFontByName( value.getString() ));
    }
}

bool UIButton::toucheBegan( const std::list<GXTouch> &)
{
    if( _type == Momentary )
    {
        changeStateTo(Selected );
    }
    else // Toggle
    {
        if( _currentState == Normal )
            changeStateTo( Selected );
        
        else if(_currentState == Selected )
            changeStateTo( Normal );
    }
        
    return true;
}

bool UIButton::toucheMoved( const std::list<GXTouch> &)
{
    return false;
}

bool UIButton::toucheEnded( const std::list<GXTouch> &)
{
    if( _target )
        _target->performSelectorWithArguments( _selector , { Variant( getCurrentState() ) } );

    if( _type == Momentary)
        changeStateTo(Normal);

    return true;
}


#ifdef ENABLE_ELEMENT_SELECTOR
const Variant UIButton::performSelectorWithArguments( const std::string &selector , const VariantList  &arguments , bool *validCall )
{
    if( validCall)
        *validCall = true;
    
    if (selector =="setText")
    {
        setText( arguments.at(0).getString() );
        return Variant::null();
    }
    
    else if( selector == "setType")
    {
        setType( (Type) arguments.at(0).getInt() );
        
        return Variant::null();
    }
    else if( selector == "getType")
    {
        return Variant( getType() );
    }
    

    
    if( validCall)
        *validCall = false;
    
    return UIView::performSelectorWithArguments(selector, arguments, validCall);
}
#endif




