//
//  UITarget.cpp
//  JSCenter
//
//  Created by Manuel Deneu on 23/11/2015.
//  Copyright © 2015 Manuel Deneu. All rights reserved.
//

#include "UIView.h"

#include "../Env_Variables.h"

#include "UI.h"
#include "UIViewController.h" 

UIView::UIView():
_touchesEnabled (true ),
_target( nullptr ),
_viewController ( nullptr )
{}


void UIView::resetState()
{
    
}
/*
void UIView::setPosition( const GXPoint &)
{
    
}
*/
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void UIView::elementWillAppear()
{
    viewWillAppear();
    
    if( _viewController)
        _viewController->viewWillAppear( this );
}

void UIView::elementDidAppear()
{
    viewDidAppear();
    
    if( _viewController)
        _viewController->viewDidAppear( this );
}

void UIView::elementWillDisappear()
{
    viewWillDisappear();
    
    if( _viewController)
        _viewController->viewWillDisappear( this );
}

void UIView::elementDidDisappear()
{
    viewDidDisappear();
    
    if( _viewController)
        _viewController->viewDidDisappear( this );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void UIView::viewWillAppear()
{
}

void UIView::viewDidAppear()
{
}

void UIView::viewWillDisappear()
{
}

void UIView::viewDidDisappear()
{   
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool UIView::toucheBegan( const std::list<GXTouch> &touches )
{
    if( !_touchesEnabled)
        return false;
    
    for (GXElement *child : getChildrenAscending() )
    {
        std::list<GXTouch> childTouches;
        
        for (const GXTouch &t : touches)
        {
            if (rectContainsPoint(child->getBounds(), t.center) )
            {
                //                printf("send touche to target %s\n"  , child->getElementName().c_str() );
                childTouches.push_back( GXTouch{ makePoint(t.center.x - child->getBounds().origin.x,
                                                           t.center.y - child->getBounds().origin.y),
                                                 t.id,
                                                 t.touchState
                                                }
                                       );
            }
        }
        if (!childTouches.empty())
        {
            UIView* target = dynamic_cast< UIView*>( child );
            if( target && target->touchesEnabled() )
            {
                
                if( target->toucheBegan( childTouches ))
                    break;

            }
        }
    }
    
    return true;
}

bool UIView::toucheMoved( const std::list<GXTouch> &touches )
{
    if( !_touchesEnabled)
        return false;

    for (GXElement *child : getChildrenAscending() )
    {
        std::list<GXTouch> childTouches;
        
        for (const GXTouch &t : touches)
        {
            if (rectContainsPoint(child->getBounds(), t.center) )
            {
                childTouches.push_back( GXTouch{ makePoint(t.center.x - child->getBounds().origin.x,
                                                           t.center.y - child->getBounds().origin.y),
                                                t.id,
                                                t.touchState
                                                }
                                       );
            }
        }
        if (!childTouches.empty())
        {
            UIView* target = dynamic_cast< UIView*>( child );
            if( target && target->touchesEnabled() )
            {
                if( target->toucheMoved( childTouches ))
                    break;
            }
        }
    }
    
    return true;
}

bool UIView::toucheEnded( const std::list<GXTouch> &touches )
{
    if( !_touchesEnabled)
        return false;
    
    for (GXElement *child : getChildrenAscending() )
    {
        std::list<GXTouch> childTouches;
        
        for (const GXTouch &t : touches)
        {
            if (rectContainsPoint(child->getBounds(), t.center) )
            {
                childTouches.push_back( GXTouch{ makePoint(t.center.x - child->getBounds().origin.x,
                                                           t.center.y - child->getBounds().origin.y),
                                                 t.id,
                                                 t.touchState
                                                }
                                       );
            }
        }
        if (!childTouches.empty())
        {
            UIView* target = dynamic_cast< UIView*>( child );
            if( target && target->touchesEnabled() )
            {
                if( target->toucheEnded( childTouches ))
                    break;
            }
        }
    }
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

#ifdef ENABLE_ELEMENT_SELECTOR
const Variant UIView::performSelectorWithArguments( const std::string &selector , const VariantList  &arguments , bool *validCall)
{
    if( validCall )
        *validCall = true;
    
    if( selector == "setViewController")
    {
        UIViewController* controller = dynamic_cast<UIViewController*>( Element::getElementById( arguments.at(0).getUInt64() ));
        
        setViewController( controller );
        
        return Variant::null();
    }
    else if( selector == "setTarget")
    {
        _selector = arguments.at(0).getString();
        _target = Element::getElementById( arguments.at(1).getUInt64() );
        
        return Variant( _target != nullptr );
    }
    else if( selector == "setPosition")
    {

        moveTo( makePoint( arguments.at(0).getInt() , arguments.at(1).getInt() ));
        return Variant::null();
    }
    
    if( validCall )
        *validCall = false;
    
    return GXScene::performSelectorWithArguments(selector, arguments , validCall );
}

#endif

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

#ifdef USE_JSON_PARSER
/*static*/ UIView* UIView::createUIElementFromJSON( const JSON::Node &node )
{
    if( !node.getObjectItem(UIAttribute::Type).isString())
    {
        printf("Error in createUIElementFromJSON : Node '%s' not found \n " , UIAttribute::Type.c_str() );
        return nullptr;
    }
    
    UIView* element = nullptr;

    const std::string cType = node.getObjectItem( UIAttribute::Type ).getString();


    if( cType == ClassNames::UIButton )
    {
        element = UIButton::createFromJSON(node);
    }
    
    else if( cType == ClassNames::UISlider )
    {
        element = UISlider::createFromJSON( node);
    }
    
    else if( cType == ClassNames::UIImage )
    {
        element = UIImage::createFromJSON( node );
    }
    
    else if( cType == ClassNames::UIVideo )
    {
        element = UIVideo::createFromJSON( node );
    }
    else
    {
        printf("Error in createUIElementFromJSON : unknown type '%s' \n" , cType.c_str() );
        return nullptr;
    }
    
    
    element->parseJSONBase( node );
    /* UIView */
    

    return element;
}

bool UIView::parseJSONBase( const JSON::Node &node )
{
    if( node.isInvalid())
        return false;

    // name
    const std::string cName = node.getObjectItem( UIAttribute::Name ).getString();
    setElementName( cName );

    const JSON::Node &touchNode = node.getObjectItem( UIAttribute::TouchesEnabled );
        
    if( touchNode.isValid())
        setTouchesEnabled( touchNode.getBool() );
    
    // selector
    std::string cSelector ="";
    
    if( node.getObjectItem( UIAttribute::Selector ).isValid() )
        cSelector = node.getObjectItem( UIAttribute::Selector).getString();
    else
        printf(" Node 'selector' invalid for %s (class %s) \n" ,getElementName().c_str()  , getClassName().c_str() );
    
    // target
    Element* target = nullptr;
    
    const JSON::Node &targetNode = node.getObjectItem( UIAttribute::Target );
    
    if( targetNode.isNumber())
        target = Element::getElementById( static_cast<element_id>( targetNode.getInt() ) );
    
    else if( targetNode.isString() )
        target = Element::getElementByName( targetNode.getString() );


    
    if( node.getObjectItem( UIAttribute::Size ).isArray() )
    {
        const int w = node.getObjectItem( UIAttribute::Size ).getArrayItem(0).getInt();
        const int h = node.getObjectItem( UIAttribute::Size ).getArrayItem(1).getInt();
    
        setBounds(makeRect(0, 0, w, h));
    }
    
    if( node.getObjectItem( GXElementAttributes::Bounds ).isArray() )
    {
        const int x = node.getObjectItem( GXElementAttributes::Bounds ).getArrayItem(0).getInt();
        const int y = node.getObjectItem( GXElementAttributes::Bounds ).getArrayItem(1).getInt();
        const int w = node.getObjectItem( GXElementAttributes::Bounds ).getArrayItem(2).getInt();
        const int h = node.getObjectItem( GXElementAttributes::Bounds ).getArrayItem(3).getInt();
        
        setBounds(makeRect( x, y, w, h));
    }
    /*
    // position
    const int x = node.getObjectItem( UIAttribute::Position ).getArrayItem(0).getInt();
    const int y = node.getObjectItem( UIAttribute::Position ).getArrayItem(1).getInt();
    
    moveTo( makePoint( x , y));
    */
    if( node.getObjectItem(GXElementAttributes::Layer).isNumber())
        setLayer( node.getObjectItem(GXElementAttributes::Layer).getInt() );
    
    
    if( cSelector.empty() )
        printf("No Selector defined for %s (class %s) \n" ,getElementName().c_str()  , getClassName().c_str() );
    
    if( !target)
        printf("No target defined for %s (class %s) \n" ,getElementName().c_str()  , getClassName().c_str() );
    
    if( !cSelector.empty() && target )
        setTarget( cSelector, target );
    
    
    return true;
}
bool UIView::saveJSONSpec( JSON::Node &) const
{
    return true;
}
bool UIView::saveJSON( JSON::Node &node) const
{
    node.addItemToObject(Variant( getElementName() ), UIAttribute::Name );
    node.addItemToObject(Variant( getClassName() ) , UIAttribute::Type);

    node.addItemToObject( Variant( getBounds().origin), UIAttribute::Position);
    node.addItemToObject( Variant(getLayer() ), GXElementAttributes::Layer );
    
    
    if( !_selector.empty() )
        node.addItemToObject(Variant(_selector ), UIAttribute::Selector );
    
    if( _target != nullptr )
        node.addItemToObject( Variant( _target->getElementId() ), UIAttribute::Target );
    
    if( !touchesEnabled())
        node.addItemToObject( Variant( false ), UIAttribute::TouchesEnabled );
    
    saveJSONSpec( node);
    return true;
}
#endif



