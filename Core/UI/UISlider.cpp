//
//  UISlider.cpp
//  JSCenter
//
//  Created by Manuel Deneu on 24/11/2015.
//  Copyright © 2015 Manuel Deneu. All rights reserved.
//

#include "../Env_Variables.h"
#include "UISlider.h"
#include "UIJSON.h"

GXSize  UISlider::DefaultSize      = makeSize(250, 50);

UISlider::UISlider():
_painter ( new GXAsyncPainter() ),
_cursor  ( new GXAsyncPainter() ),
_value( 0.5f)
{
    setElementName( ClassNames::UISlider );
    className = ClassNames::UISlider;
    
    setTransparency(true);
    setSize( DefaultSize );
    
    _painter->setLayer(0);
    _painter->setTransparency(true);
    _painter->setSize(getBounds().size);
    
    _painter->setFillColor( makeColor(120, 0, 0)  );
    _painter->setStrokeColor( makeColor(120, 0, 0) );
    
    _painter->addRoundedRect( makeRect(makePointNULL(), getBounds().size ), 15, 15);
    _painter->fill();
    _painter->stroke();
    
    _cursor->setLayer(1);
    _cursor->setTransparency(true);
    _cursor->setSize(makeSize(20, getBounds().size.height ));
    
    _cursor->setFillColor( makeColor(0, 120, 0)  );
    _cursor->setStrokeColor( makeColor(0, 120, 0) );
    
    _cursor->addRoundedRect( makeRect(makePointNULL(), _cursor->getBounds().size), 15, 15);
    _cursor->fill();
    _cursor->stroke();
    
    
    
    
    addElement( _painter);
    addElement( _cursor );
}

UISlider::~UISlider()
{
    
}

void UISlider::setPosition( const GXPoint &pt)
{
    setBounds(makeRect(pt, getBounds().size ) );
    _painter->setBounds( getBounds() );
    _cursor->setBounds( getBounds() );

    

    setNeedsDisplay();
}

void UISlider::setValue( float val)
{
    
    _value = val;
    
    GXRect r = getBounds();
    
    int pos = static_cast<int>( getBounds().size.width*val );
    
    if (pos <0)
        pos = 0;
    if( pos>getBounds().size.width)
        pos = getBounds().size.width;
    
    r.origin.x += pos;
    
    _cursor->setBounds(r);
    _cursor->setNeedsDisplay();
    
    setNeedsDisplay();
}



bool UISlider::toucheBegan( const std::list<GXTouch> & )
{
    return true;
}

bool UISlider::toucheMoved( const std::list<GXTouch> &touches)
{
    const GXPoint pt = touches.front().center;
    
    GXRect r = getBounds();
    r.origin.x += pt.x;
    _cursor->setBounds(r);
    _cursor->setNeedsDisplay();
    
    _value =(float) pt.x / getBounds().size.width ;

    if( _target )
    {
        Dispatch::getInstance()->performAsync(this, _target, _selector, { Variant( _value) } );
        
        //_target->performSelectorWithArguments( _selector, { Variant( _value) });
    }
    
    return true;
}

bool UISlider::toucheEnded( const std::list<GXTouch> &)
{
//    printf("Touch ended in UISlider\n ");
    return true;
}

#ifdef ENABLE_ELEMENT_SELECTOR
const Variant UISlider::performSelectorWithArguments( const std::string &selector , const VariantList  &arguments , bool *validCall )
{
    if( validCall)
        *validCall = true;

    
    if( selector == "setValue" )
    {
        setValue( arguments.at(0).getFloat() );
        return Variant::null();
    }
    
    else if ( selector =="getValue")
        return Variant( getValue() );
    
    if( validCall)
        *validCall = false;
    
    return UIView::performSelectorWithArguments(selector, arguments, validCall);
}
#endif


