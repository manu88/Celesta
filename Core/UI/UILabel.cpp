//
//  UILabel.cpp
//  JSCenter
//
//  Created by Manuel Deneu on 27/11/2015.
//  Copyright © 2015 Manuel Deneu. All rights reserved.
//

#include "UILabel.h"
#include "../GX/GXFont.h"
#include "../Env_Variables.h"

UILabel::UILabel():
_text    ( new GXTextRenderer() )
{
    setElementName( ClassNames::UILabel );
    className = ClassNames::UILabel;
    
    
    setTransparency(true);
    setSize( makeSize(150, 70) );
    

    
    _text->setFont( GXFont::getFontByName( DefaultsValues::DefaultFont ) );
    _text->setTextColor( makeColor(0, 0, 0) );
    
    // size 10 pour height 100 ,
    _text->setSizeInPoints( getBounds().size.height/10 );
    
    _text->setLayer(1);
    _text->setTransparency( true );
    _text->setSize( getBounds().size );
    
    setText( getElementName() );
    
    addElement( _text );
    
}

UILabel::~UILabel()
{
    
}

void UILabel::setPosition( const GXPoint &pt)
{
    setBounds(makeRect(pt, getBounds().size ) );
    

    _text->setBounds( makeRect(
                               makePoint(pt.x, pt.y + getBounds().size.height ),
                               getBounds().size
                               )
                     );
    

    setNeedsDisplay();
}

void UILabel::setText( const std::string &text )
{
    _text->setDisplayedText( text );
    _text->setUnprepared();
}


#ifdef ENABLE_ELEMENT_SELECTOR
const Variant UILabel::performSelectorWithArguments( const std::string &selector , const VariantList  &arguments , bool *validCall )
{
    if( validCall)
        *validCall = true;
    
    if (selector =="setText")
    {
        setText( arguments.at(0).getString() );
        return Variant::null();
    }
    
    
    if( validCall)
        *validCall = false;
    
    return UIView::performSelectorWithArguments(selector, arguments, validCall);
}
#endif

#ifdef USE_JSON_PARSER
bool UILabel::saveJSONSpec( JSON::Node &node) const
{
    return true;
}
#endif

