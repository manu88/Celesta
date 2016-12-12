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

#ifdef USE_JSON_PARSER

/*static*/ UIWindow* UIWindow::createFromJSON( const JSON::Node &node)
{
    UIWindow* win  = new UIWindow();
    
    win->parseJSON( node );
    
    return win;
}

bool UIWindow::parseJSON( const JSON::Node &node )
{
    const std::string name = node.getObjectItem(UIAttribute::Name).getString();

    setElementName(name);
    
    const JSON::Node &children = node.getObjectItem( UIAttribute::Children );
    
    if( !children.isArray())
    {
        return false;
    }
    
    for( const JSON::Node & child : children)
    {
        UIView* element = UIView::createUIElementFromJSON( child);
        
        if( element != nullptr)
        {
            addElement( element );
        }

    }
    
    
    return false;
}

bool UIWindow::saveJSON( JSON::Node &node) const
{
    node.addItemToObject(Variant(getElementName()), UIAttribute::Name );
    
    JSON::Node children  = node.addArray( UIAttribute::Children );
    
    for (const GXElement *c : getChildren() )
    {
        const UIView* view = dynamic_cast<const UIView*>( c );
        
        JSON::Node child;
        
        if( view->saveJSON( child) )
            children.addItemToArray(child);
    }
    
    return true;
}
#endif

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

#ifdef USE_JSON_PARSER
bool UIStoryBoard::parseJSON( const JSON::Node &node)
{
    if( node.isInvalid())
        return false;
    
    if( !node.getObjectItem( UIAttribute::Name ).isString())
        return false;
    
    const std::string title = node.getObjectItem( UIAttribute::Name ).getString();
    
    setElementName(title);
    
    const JSON::Node &views = node.getObjectItem( UIAttribute::Children );
    
    if( !views.isArray())
    {
        return false;
    }
    
    for( const JSON::Node & child : views)
    {
        UIWindow* childWin = UIWindow::createFromJSON( child );
        
        if( childWin != nullptr)
            addWindow( childWin );

    }

    return true;
}

bool UIStoryBoard::saveJSON( JSON::Node &node) const
{
    
    node.addItemToObject(Variant(getElementName()), UIAttribute::Name );
    
    JSON::Node children  = node.addArray( UIAttribute::Children );
    for (const UIWindow *win : _windows )
    {
        JSON::Node child;
        
        if( win->saveJSON( child) )
            children.addItemToArray(child);
    }
    
    return true;
}


#endif

#ifdef ENABLE_ELEMENT_SELECTOR
const Variant UIStoryBoard::performSelectorWithArguments( const std::string &selector , const VariantList  &arguments, bool *validCall )
{
    if( validCall)
        *validCall = true;
    
    if( selector == "getNumWindows")
        return Variant( getNumWindows() );
    
#ifdef USE_JSON_PARSER
    else if( selector == "parseJSONFile")
    {
        JSON::Document doc;
        if(!doc.parseFile( arguments.at(0).getString() ))
        {
            printf("JSON parse error '%s' \n" , doc.getError().c_str() );
            return Variant( false );
        }
        
        return Variant( parseJSON( doc.getRootNode() ) );
    }
    else if( selector == "saveJSON")
    {
        JSON::Document doc;

        if( saveJSON( doc.getRootNode() ))
        {
            doc.saveFile( arguments.at(0).getString() , false);
            return Variant( true );
        }

        return Variant( false );
    }
#endif
    
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

