//
//  UI.cpp
//  JSCenter
//
//  Created by Manuel Deneu on 23/11/2015.
//  Copyright © 2015 Manuel Deneu. All rights reserved.
//

#include "UI.h"


#include "../GX/DisplayController.h"



/* **** **** **** **** **** **** **** **** **** **** **** **** */

UIWindow::UIWindow()
{
    setElementName( "UIWindow");
    className ="UIWindow";

    /**/
    
    setOpacity(true);
    setBackgroundColor( UIDefaults::BackgroundColor );
    setBounds( makeRect(makePointNULL(),  DisplayController::getInstance()->getCurrentMode().size ) );
    
    GXTouchController::retain();
    
}

UIWindow::~UIWindow()
{
    GXTouchController::release();
    deleteRessources();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */


void UIWindow::setToDefaultValues()
{
    for (GXElement *child : getChildren() )
    {
        UIView* c = dynamic_cast<UIView*>(child);
        c->resetState();
    }
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void UIWindow::viewWillAppear()
{
}

void UIWindow::viewDidAppear()
{
    GXTouchController::getInstance()->setDelegate( this );
}

void UIWindow::viewWillDisappear()
{
}

void UIWindow::viewDidDisappear()
{
}

#ifdef ENABLE_ELEMENT_SELECTOR
const Variant UIWindow::performSelectorWithArguments( const std::string &selector , const VariantList  &arguments , bool *validCall )
{
    if( validCall)
        *validCall = true;
    
    if( validCall)
        *validCall = false;
    
    return UIView::performSelectorWithArguments(selector, arguments, validCall );
}
#endif

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

UIStoryBoard::UIStoryBoard()
{
    setElementName( ClassNames::UIStoryBoard );
    className = ClassNames::UIStoryBoard;
    
}

UIStoryBoard::~UIStoryBoard()
{
    for( UIWindow *w : _windows )
    {
        delete w;
    }
    _windows.clear();
}

#ifdef ENABLE_ELEMENT_SELECTOR
const Variant UIStoryBoard::performSelectorWithArguments( const std::string &selector , const VariantList  &arguments, bool *validCall )
{
    if( validCall)
        *validCall = true;
    
    if( selector == "getNumWindows")
        return Variant( getNumWindows() );
    

    
    else if( selector == "getWindowNamed")
    {
        const UIWindow* win = getWindowNamed( arguments.at(0).getString() );
        
        if( win)
            return Variant( win->getElementId() );
        
        return Variant::null();
    }
    
    if( validCall)
        *validCall = false;
    
    return Element::performSelectorWithArguments(selector, arguments, validCall );
}
#endif

