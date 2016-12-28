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





