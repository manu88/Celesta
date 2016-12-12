
//
//  GXScene.cpp
//  
//
//  Created by Manuel Deneu on 31/07/14.
//
//


#include <stdio.h>
#include <algorithm>    
#include <assert.h>
#include <limits.h>

#include "../Config.h"
#include "../Env_Variables.h"
#include "DisplayController.h"

#include "GXScene.h"
#include "GXPath.h"

/* **** **** **** **** **** **** **** **** **** **** **** */

GXScene::GXScene():
_ownsElements( true )
{
    className = ClassNames::GXScene;
}

GXScene::~GXScene()
{
}

/* **** **** **** **** **** **** **** **** **** **** **** */

bool GXScene::addElement( GXElement* element )
{


    if ( isAlreadyInScene( element ) )
        return false;
    
    element->setParentElement( this );
    
    setUnprepared();
    _elements.push_back( element );
    element->changeCycleTo( WillAppear );
    reorderElements();
    
    element->setNeedsDisplay();
    
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** */

bool GXScene::removeElement( GXElement* element )
{
    if (element == nullptr)
        return false;
    
    auto got = std::find (_elements.begin() , _elements.end() , element );

    if ( got == _elements.end() )
        return false;
    
    reorderElements();

    element->removeFromParent();
    


    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** */

void GXScene::removeAllElements()
{
    for ( auto i : _elements )
    {
        i->removeFromParent();
    }
}

/* **** **** **** **** **** **** **** **** **** **** **** */

const GXElement* GXScene::childAtPoint( const GXPoint &pt) const
{
    for (const GXElement* e : _childrenAscending )
    {
        if( rectContainsPoint( e->getBounds() ,pt ) )
            return e;
    }
    return nullptr;
}

/* **** **** **** **** **** **** **** **** **** **** **** */

GXElement* GXScene::getElementByID( element_id _id) const
{

    for ( const auto &i : _elements )
    {
        if ( i->getElementId() == _id )
            return i;
    }
    
    return nullptr;
}

/* **** **** **** **** **** **** **** **** **** **** **** */

const GXElement* GXScene::getElementByName( const std::string &name ) const
{
    for ( const auto &i : _elements )
    {
        if ( i->getElementName() == name )
            return i;
    }
    
    return nullptr;
}

/* **** **** **** **** **** **** **** **** **** **** **** */

bool GXScene::isAlreadyInScene( GXElement* elementTofind ) const
{
    auto iter = std::find(_elements.begin() , _elements.end() ,  elementTofind );
    
    return iter != _elements.end();
}



/* **** **** **** **** **** **** **** **** **** **** **** */

void GXScene::forceUpdate()
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXScene::reorderElements()
{
    std::sort(_elements.begin() , _elements.end(), layer_comparor() );
    _childrenAscending.clear();
    _childrenAscending = _elements;
    
    std::sort(_childrenAscending.begin() , _childrenAscending.end(), layer_comparorAscending() );
}

/* **** **** **** **** **** **** **** **** **** **** **** */

int GXScene::findDeepestLayer() const
{
    int min = INT_MAX;
    
    for ( const GXElement *c : getChildren())
    {
        if ( c->getLayer() <= min)
            min = c->getLayer();
        
    }
    
    return min;
    
}

int GXScene::findTopLayer() const
{
    int max = INT_MIN;
    
    
    for ( const GXElement *c : getChildren())
    {
        if ( c->getLayer() >= max)
            max = c->getLayer();
        
    }
    
    return max;
}

bool GXScene::needsDisplay() const
{
    for ( const auto &child : _elements )
    {
        if ( child->needsDisplay() )
            return true;
    }
    return _needsDisplay;
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

// ALWAYS called on GUI Thread !

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */


void GXScene::paint( const GXRect &rect , GXAnimation* /*anim*/ )
{
    reorderElements();
    
    
    if(isOpaque())
        GXPath::clearRect( getBounds(), getBackgroundColor() );
    
    if ( _elements.empty() )
    {
        GXPath::clearRect( getBounds(), getBackgroundColor() );
        setUpdated();
    }
    
    for ( auto &i : _elements )
    {
        updateElementInRect( i , rect );
    }
    
    for ( auto &i : _elements )
    {
        i->setUpdated();
        
    }

}

/* **** **** **** **** **** **** **** **** **** **** **** */

void GXScene::updateElementInRect( GXElement *element , const GXRect &rect )
{
    if ( !element->isPrepared() )
    {
        element->prepare();

    }
    
    if ( element->_changed_flag && element->m_callChangedOnGUIThread)
    {
        element->changed();
        element->_changed_flag = false;
    }
    
    
    if ( element->shouldBeRemoved() )
    {
        element->cleanUp();
        element->changeCycleTo( CycleState::NotVisible);
        
        auto it = std::find(_elements.begin(), _elements.end(), element);
        if( it != _elements.end())
            _elements.erase( it);

    }
    
    else if (/* element->needsDisplay() && */element->isVisible() )
    {
        if( element->_elementCycleState != GXElement::CycleState::Visible )
            element->changeCycleTo( GXElement::CycleState::Visible);

        /*
        if( element->_moved )
            GXPath::clearRect( element->_lastBounds, element->getBackgroundColor() );
         */
        element->update(element->getBounds(), element->getParentElement()->_anim);
        /*
        for (auto prev : _elements)
        {

            if (element->_moved)
            {
                prev->update( element->_lastBounds, element->getParentElement()->_anim);
                element->_moved = false;
            }
            
            const GXRect rr = rectIntersection( element->_updateRect , prev->_bounds ) ;
            
            if ( rr.isValid() )
            {
                prev->update(rr, element->getParentElement()->_anim);// updateElementInRect( prev,rr);
            }
        }
         */
    }
}

void GXScene::prepareRessources()
{

    for ( auto i  : _elements )
    {
        if ( !(i->isPrepared() ) )
            i->prepare();
        
    }
}

void GXScene::deleteRessources()
{
    for ( auto i  : _elements )
    {
        i->cleanUp();
        if( _ownsElements)
            delete i;
    }
    
    _elements.clear();
}

void GXScene::changed()
{
    /*
    if( _moved )
    {
        for ( auto &&elem : _elements )
        {
            elem->moveOf(_bounds.origin.x - _lastBounds.origin.x , _bounds.origin.y - _lastBounds.origin.y);
        }
    }
     */
    reorderElements();

}

void GXScene::deleteElementFromGUIThread(GXElement *element)
{
    if (element == nullptr)
        return;
    
    auto got = std::find (_elements.begin() , _elements.end() , element );
    
    if ( got == _elements.end() )
        return;
    
    delete element;
    
    _elements.erase( got );

}

void GXScene::childRemoved( GXElement *element )
{
    if (element == nullptr)
        return;
    
    auto got = std::find (_elements.begin() , _elements.end() , element );
    
    if ( got == _elements.end() )
        return;
    
    
    _elements.erase( got );
}

#ifdef USE_JSON_PARSER
bool GXScene::addJSONinfos( JSON::Node &node) const
{
    
    JSON::Node child = node.addArray("children");
    
    for (const auto &el : getChildren() )
    {
        JSON::Node me = el->initNode();

        el->addJSONinfos( me );
        child.addItemToArray( me );
        
    }
    return true;
}
#endif


#ifdef ENABLE_ELEMENT_SELECTOR
const Variant GXScene::performSelectorWithArguments( const std::string &selector , const VariantList  &arguments , bool *validCall)
{
    if( validCall)
        *validCall = true;
    
    if( selector == "addElement")
    {
        GXElement *e = dynamic_cast<GXElement*>(Element::getElementById( arguments.at(0).getUInt64() ) );

        if (e)
            return Variant( addElement( e) );
        
        return Variant::null();
    }

    else if( selector == "removeElement")
    {
        GXElement *e = dynamic_cast<GXElement*>(Element::getElementById( arguments.at(0).getUInt64() ) );

        if (e)
            return Variant( removeElement( e) );

        return Variant::null();
        
    }
    else if( selector == "removeAllElements")
    {
        removeAllElements();
        return Variant::null();
    }
    
    else if( selector =="getNumChildren")
    {
        return Variant( static_cast<unsigned long>( _elements.size() ) );
    }
    else if( selector == "getChildAt")
    {
        const GXElement* c = _elements.at( arguments.at(0).getULong() );
        
        if( c )
            return Variant( c->getElementId()  );
        
        return Variant::null();
    }
    
    else if( selector == "getChildNamed")
    {
        for( const GXElement*c : _elements )
        {
            if( c->getElementName() == arguments.at(0).getString() )
                return Variant( c->getElementId() );
        }
        
        return Variant::null();
    }
    
    else if( selector == "childAtPoint")
    {
        const GXElement* c = childAtPoint( makePoint( arguments.at(0).getInt(),
                                                      arguments.at(1).getInt()
                                                     )
                                          );
        if( c)
            return Variant( c->getElementId() );

        return Variant::null();
    }
    
    if( validCall)
        *validCall = false;
    
    return GXElement::performSelectorWithArguments(selector, arguments, validCall);
}
#endif
