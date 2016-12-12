//
//  UINotification.cpp
//  JSCenter
//
//  Created by Manuel Deneu on 27/11/2015.
//  Copyright © 2015 Manuel Deneu. All rights reserved.
//

#include "UINotification.h"
#include "UI.h"
#include "../Env_Variables.h"

#include "../GX/DisplayController.h"

UIModalAlert::UIModalAlert( UIWindow* parent ):
_window       ( parent ),
_rect         ( new GXAsyncPainter() ),
_title        ( new UILabel() ),
_okButton     ( new UIButton() ),
_cancelButton ( new UIButton() ),

_ret ( -1 )
{
    setParentElement( parent );
    
    setElementName( ClassNames::UIModalAlert );
    className = ClassNames::UIModalAlert;
    
    setBounds( _window->getBounds() );
    const GXSize alertSize = makeSize(400, 200);
    
    const GXRect bounds = _window->getBounds();
    const GXPoint center = makePoint( bounds.size.width /2, bounds.size.height/2);
    
    setTransparency( true);
    
    _winBounds= makeRect(center.x - (alertSize.width  / 2 ), center.y - (alertSize.height / 2 ), alertSize.width, alertSize.height );
    
    _okButton->setTarget("okTouched", this );
    _cancelButton->setTarget("cancelTouched", this );
    
}

UIModalAlert::~UIModalAlert()
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ int UIModalAlert::showModalAlert(UIWindow *parent , const std::string &title )
{
    if( !parent)
        return -1;
    
    UIModalAlert *alert = new UIModalAlert(parent);
    
    alert->setTitle( title );
    
    if( !alert->create())
    {
        delete alert;
        return -1;
    }
    
    const int ret = alert->show();
    

    return ret;
}

void UIModalAlert::setTitle( const std::string &title)
{
    _title->setText( title );
}

bool UIModalAlert::create()
{
    if( !_window)
        return false;
    
    _rect->setLayer( 0 );
    _title->setLayer( 1 );
    _okButton->setLayer( 2 );
    _cancelButton->setLayer( 2 );

    _okButton->setText("ok");
    _cancelButton->setText("cancel");
    
    _title->setPosition( makePoint( _winBounds.origin.x , _winBounds.origin.y + 130 ) );
                        
    _okButton->setPosition( makePoint( _winBounds.origin.x, _winBounds.origin.y  + 0 ));
    _cancelButton->setPosition( makePoint( _winBounds.origin.x + 160 , _winBounds.origin.y  + 0 ));
    
    _rect->setBounds(getBounds() );
    
    _rect->addRect(getBounds() );
    _rect->setFillColor( makeColor( 0, 0, 0 , 127));
    _rect->fill();
    _rect->addRoundedRect( _winBounds, 30, 30);
    _rect->setFillColor( makeColor(0, 0, 180 , 127 ));
    _rect->fill();
    
    
    addElement( _rect );
    addElement( _title );
    addElement( _okButton );
    addElement( _cancelButton );
    
    return true;
}

int UIModalAlert::show()
{
    setLayer( _window->findTopLayer() +1 );
    
    _window->addElement( this );
    
    _shouldReturn = false;
    
    Dispatch::getInstance()->runModalLoopForElement( this , &_shouldReturn );
        
    _window->removeElement( this );

    return _ret;
}


#ifdef ENABLE_ELEMENT_SELECTOR
const Variant UIModalAlert::staticSelector(const std::string& selector , const VariantList& arguments, bool *validCall)
{
    if( validCall)
        *validCall = true;
    
    if( selector == "showModalAlert")
    {
        UIWindow *parent = dynamic_cast<UIWindow*>( DisplayController::getInstance()->getCurrentElement() );
        
        const std::string &title = arguments.at(0).getString();
        
        return Variant( showModalAlert(parent, title) );
    }
    
    if( validCall)
        *validCall = false;
    return Element::staticSelector(selector, arguments, validCall );
}

const Variant UIModalAlert::performSelectorWithArguments( const std::string &selector , const VariantList  &arguments , bool *validCall)
{
    if( validCall )
        *validCall = true;
    
    if( selector == "okTouched")
    {
        _ret = 1;
        _shouldReturn = true;
    }
    else if( selector == "cancelTouched")
    {
        _ret = 2;
        _shouldReturn = true;
    }
    if( validCall )
        *validCall = false;
    
    return UIView::performSelectorWithArguments(selector, arguments, validCall);
}

#endif
