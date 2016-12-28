//
//  GXElement.cpp
//  
//
//  Created by Manuel Deneu on 31/07/14.
//
//

#include <assert.h>

#include "DisplayController.h"
#include "../Scheduler/Dispatch.h"
#include "GXElement.h"
#include "GXPath.h"
#include "GXTransformations.h"
#include "GXAnimation.h"

/* **** **** **** **** **** **** **** **** **** **** **** */

GXElement::GXElement() :
    _layer                    ( 0       ),
    _alpha                    ( 255     ),
    _prepared                 ( false   ),
    _shouldBeRemoved          ( false   ),
    _added                    ( false   ),
    _needsDisplay             ( false   ),
    _updateRect               ( makeRectNULL() ),
    _hidden                   ( false   ),

    _layerChanged             ( false   ),
    _changed_flag             ( false   ),
    m_callChangedOnGUIThread  ( true    ),

    _parentElement            ( nullptr ),
    _bounds                   ( makeRectNULL()       ),
    _backgroundColor          ( makeColor( 0, 0, 0 ) ),
    _isTransparent            ( false   ),
    _autoRendering            ( false   ),

//    _moved                    ( false   ),
    _lastBounds               ( makeRectNULL() ),
    _anim                     ( nullptr ),
    _animCompletionEvent      ( nullptr ),

    _transformation           ( nullptr ),


    _elementCycleState        ( NotVisible )

{

    addTypeInfo( Video );
    
    addValueForKey(GXElementAttributes::Bounds, Variant( { Variant(_bounds.origin.x ),
                                                         Variant(_bounds.origin.y ),
                                                         Variant(_bounds.size.width ),
                                                         Variant(_bounds.size.height )
                                                      })
                   );
}

/* **** **** **** **** **** **** **** **** **** **** **** */

GXElement::~GXElement()
{
    if( _anim)
    {
        delete _anim;
    }
}

/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::performChangedSignalOnGUIThread( bool callOnGUIThread )
{
    m_callChangedOnGUIThread  = callOnGUIThread;
}

/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::prepare()
{

    prepareRessources();
    
    _updateRect =  _bounds;
    
    _prepared = true;
}

/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::cleanUp()
{

    deleteRessources();

    if( _transformation )
    {
        delete _transformation;
        _transformation = nullptr;
    }
}

/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::setLayer(int layer)
{
    if ( _layer != layer )
    {
        _layer        = layer;
        _layerChanged = true;
        
        
        updateValueForKey( GXElementAttributes::Layer, Variant(_layer ));
        elementChanged();
    }
}

int GXElement::getLayer() const
{
    return _layer;
}

/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::setBounds( const GXRect &bounds )
{
    if( _bounds != bounds)
    {
        _bounds = bounds;
    
        //updateValueForKey(GXElementAttributes::Bounds, Variant(_bounds ));

        elementChanged();
    }
}

/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::moveTo( const GXPoint &point )
{
    setBounds( makeRect(point, getBounds().size));
    
    printf("Move %s to %i %i \n" , getElementName().c_str() , point.x , point.y );

}

void GXElement::moveOf( int dX , int dY)
{
    moveTo( makePoint( _bounds.origin.x + dX, _bounds.origin.y + dY)); // call ElementChanged!
}

void GXElement::moveTo( int x , int y)
{
    moveTo( makePoint(x, y)); // call ElementChanged!
}

/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::setSize( int width , int height)
{
    setSize( makeSize( width, height ) ); // call ElementChanged!
}

void GXElement::setSize( const GXSize &size)
{
    setBounds( makeRect(_bounds.origin, size ) );
    

}

/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::setTransparency( bool transparent)
{
    if (_isTransparent != transparent )
    {
        _isTransparent = transparent;
        
        updateValueForKey( GXElementAttributes::Transparent, Variant( _isTransparent ));
        
        elementChanged();
    }
}

void GXElement::setOpacity( bool opaque )
{
    setTransparency( !opaque ); // call ElementChanged!
}

/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::setHidden(bool hidden)
{
    if ( hidden != _hidden )
    {
        _hidden = hidden;
        
        updateValueForKey( GXElementAttributes::Hidden, Variant( _hidden ));
        elementChanged();
    }
}

void GXElement::setVisible(bool visible)
{
    setHidden( !visible ); // call ElementChanged!
}

bool GXElement::flipVisibility()
{
    setHidden( !_hidden ); // call ElementChanged!
    
    return _hidden;
}


/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::elementChanged()
{
    // direct call
    if ( !m_callChangedOnGUIThread )
    {
        changed();
        _changed_flag  = false;
    }
    
    // set flag to perform call later on GUI Thread
    else
        _changed_flag  = true;


    if (_parentElement)
        _parentElement->elementChanged();
}


// base imp. does nothing. subclasses should override this
// to get a notification of removed element
void GXElement::childRemoved( GXElement * )
{
    
}


/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::setUnprepared()
{
    _prepared = false;
}

/* **** **** **** **** **** **** **** **** **** **** **** */

void  GXElement::setNeedsDisplay()
{
    setNeedsDisplayInRectForFrames( _bounds  , 1);
}

void GXElement::setNeedsDisplayFor( int numFrames)
{
    setNeedsDisplayInRectForFrames( _bounds, numFrames );
}

/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::setNeedsDisplayInRect( const GXRect &rect)
{
    setNeedsDisplayInRectForFrames( rect , 1);
    
}

void GXElement::setNeedsDisplayInRectForFrames( const GXRect &rect , int numFrames)
{
//    ScopedLock l ( DisplayController::getInstance()->_sync);
    

    _updateRect  = rect ;
    _needsDisplay = numFrames;
    
    if( _parentElement)
        _parentElement->setNeedsDisplayInRectForFrames( rect , numFrames );

    if( !DisplayController::getInstance()->calledFromThisThread() )
        DisplayController::getInstance()->wakeUpThread();
    
}
/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::setUpdated()
{
    if (!_autoRendering)
    {
        _needsDisplay--;
        
        if (_needsDisplay<0)
        {
            _needsDisplay = 0;
        }
    }
    

}

bool GXElement::needsDisplay() const
{

    if( _anim != nullptr )
    {
        if( !_anim->isStarted() && !_anim->ended() )
            return _anim->start();
        
        if (!_anim->ended() && _anim->remainingTime() > 0 )
            return true;
        
        else
        {
            return attachAnimation( _anim->_next);
        }
     
    }
    
    if ( (_needsDisplay > 0) || _autoRendering )
    {   
        return true;
    }
    
    return false;
}

/* **** **** **** **** **** **** **** **** **** **** **** */

bool GXElement::startContinuousRendering()
{
    if ( _autoRendering )
        stopContinuousRendering();
    
    _autoRendering = true;


    _parentElement->setNeedsDisplay();
    
    return _autoRendering;
}

bool GXElement::stopContinuousRendering()
{
    _autoRendering = false;
    
    _needsDisplay = 0;
    
    return _autoRendering;
}

/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::update( const GXRect &rect, GXAnimation* anim )
{
    if ( _anim != nullptr )
    {
        if (!_anim->isStarted() )
            _anim->start();
    
        _anim->processAnimation();
    }

    
    paint( rect, anim);

    if ( _anim != nullptr )
    {
        _anim->_framesCounted++;
    }

}

/* **** **** **** **** **** **** **** **** **** **** **** */

bool GXElement::attachAnimation( GXAnimation *anim , Event *completionEvent) const
{
    GXAnimation * last = _anim;


    if( !anim)
    {

        return false;
    }
    
    _anim = anim;
    _anim->prepareAnimationForElement( const_cast<GXElement*>( this) );
    
    if (completionEvent )
        _animCompletionEvent = completionEvent;

    _anim->desc();
    
    if (last)
        delete last;
    
    return true;
}



/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::setStrokeColor( GXColor color )
{
    GXPath::setStrokeColor( color );
    
    elementChanged();
}

/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::setFillColor  ( GXColor color )
{
    GXPath::setFillColor( color );
    
    elementChanged();
}

/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::setBackgroundColor( const GXColor &color)
{
    _backgroundColor = color;
    
    GXPath::clearRect( _bounds , _backgroundColor);
}

/* **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::beginTransformation()
{
    if( _transformation != nullptr )
    {
        delete _transformation;
        _transformation = nullptr;
    }
    
    _transformation = new GXTransformation();
}

void GXElement::endTransformation()
{
    if( _transformation != nullptr )
    {
        
    }
}


/* **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** */


#ifdef ENABLE_ELEMENT_SELECTOR
const Variant GXElement::performSelectorWithArguments( const std::string &selector , const VariantList  &arguments ,bool *validCall /*= nullptr*/)
{
    if( validCall )
        *validCall = true;
    /* Setters */
    
    if ( selector == "setNeedsDisplay")
    {
        if( arguments.size() == 4)
            setNeedsDisplayInRect( makeRect( arguments.at(0).getInt(),
                                             arguments.at(1).getInt(),
                                             arguments.at(2).getInt(),
                                             arguments.at(3).getInt())
                                  );
        else
            setNeedsDisplay();

        return Variant::null();
    }
    
    /* *** *** *** *** *** *** *** *** *** *** */
    
    else if ( selector == "runAnimation")
    {
        Element *elem =  Element::getElementById(arguments.at(0).getUInt64() );
        
        if (!elem)
        {
            printf("\n runAnimation : element %i not found" ,arguments.at(0).getInt() );

            return Variant::null();
        }
        
        GXAnimation *anim = reinterpret_cast< GXAnimation* >( elem );
        printf("\n runAnimation : element %i " ,arguments.at(0).getInt() );
        printf("\n class name -> %s element Name %s" , anim->className.c_str() , anim->getElementName().c_str() );
        return Variant( attachAnimation( anim ) );
    }
    
    else if (selector == "runAnimationWithCompletion")
    {
        Element *elem =  Element::getElementById(arguments.at(0).getUInt64() );

        if (!elem)
        {
            printf("\n runAnimation : element %i not found" ,arguments.at(0).getInt() );
            
            return Variant::null();
        }
        
        GXAnimation *anim = reinterpret_cast< GXAnimation* >( elem );
        printf("\n runAnimation : element %i " ,arguments.at(0).getInt() );
        printf("\n class name -> %s element Name %s" , anim->className.c_str() , anim->getElementName().c_str() );
        return Variant( attachAnimation( anim , new EventMessage( arguments.at(1).getString() , this ) ) );
    }
    
    /* *** *** *** *** *** *** *** *** *** *** */
    
    else if ( selector == "startContinuousRendering")
    {
        return Variant ( startContinuousRendering() );
    }
    
    else if ( selector == "stopContinuousRendering")
    {
        return Variant ( stopContinuousRendering() );
    }
    
    else if ( selector == "setLayer")
    {
        setLayer( arguments.at(0).getInt() );
        return Variant::null();
    }
    else if ( selector == "setBounds" )
    {
        setBounds( makeRect( arguments.at(0).getInt(),
                             arguments.at(1).getInt(),
                             arguments.at(2).getInt(),
                             arguments.at(3).getInt()
                            )
                  );
        return Variant::null();
    }
    else if ( selector == "moveOf" )
    {
        moveOf(arguments.at(0).getInt(), arguments.at(1).getInt() );
        return Variant::null();
    }
    else if ( selector == "moveTo" )
    {
        moveTo(arguments.at(0).getInt(), arguments.at(1).getInt() );
        return Variant::null();
    }
    
    else if ( selector == "flipVisibility")
        return Variant ( flipVisibility() );
    
    else if ( selector == "setVisible")
    {
        setVisible( arguments.at(0).getBool() );
        return Variant::null();
    }
    
    else if ( selector == "setBackgroundColor")
    {
        const GXColor col = makeColor(  arguments.at(0).getInt(),
                                        arguments.at(1).getInt(),
                                        arguments.at(2).getInt(),
                                        arguments.at(3).getInt()
                                      );
        setBackgroundColor( col );
        return Variant::null();
    }
    
    else if ( selector == "setAlpha")
    {
        setAlpha( static_cast<uint8_t>( arguments.at(0).getInt() ) );
        return Variant::null();
    }
    
    else if (selector == "getAlpha")
    {
        return Variant( getAlpha() );
    }

    else if ( selector == "setTransparency")
    {
        setTransparency( arguments.at(0).getBool() );
        return Variant::null();
    }
    
    /* Getters */
    
    else if ( selector == "isVisible" )
        return Variant ( isVisible() );
    
    else if ( selector == "needsDisplay" )
        return Variant ( needsDisplay() );
    
    
    else if ( selector == "getBounds" )
        return Variant{ Variant ( getBounds().origin.x ),
                        Variant ( getBounds().origin.y ) ,
                        Variant ( getBounds().size.width) ,
                        Variant ( getBounds().size.height)
                      };

    else if ( selector == "getBackgroundColor")
        return Variant{ Variant ( getBackgroundColor().r ),
                        Variant ( getBackgroundColor().g ),
                        Variant ( getBackgroundColor().b ),
                        Variant ( getBackgroundColor().a )
                      };
    
    else if ( selector == "isOpaque")
        return Variant ( isOpaque() );
    

    /* **** **** **** **** **** **** **** **** **** */
    /*
     void setHidden(  bool hidden);
     void setSize( int width , int height);
     void setOpacity( bool opaque );
     */
    
    if( validCall )
        *validCall = false;
    
    return Element::performSelectorWithArguments(selector, arguments , validCall );
}


const Variant GXElement::staticSelector(const std::string& selector , const VariantList& arguments, bool *validCall)
{

    
    return Element::staticSelector(selector, arguments, validCall );
}

/* **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::changeCycleTo( CycleState state)
{
    if( _elementCycleState == NotVisible )
    {
        if( state == WillAppear)
        {
            _elementCycleState = WillAppear;
            elementWillAppear();
            return;
        }
    }
    else if( _elementCycleState == WillAppear )
    {
        if( state == Visible )
        {
            _elementCycleState = Visible;
            elementDidAppear();
            return;
        }
    }
    else if( _elementCycleState == Visible )
    {
        if( state == WillDiseapper)
        {
            _elementCycleState = WillDiseapper;
            elementWillDisappear();
            return;
        }
    }
    else if( _elementCycleState == WillDiseapper)
    {
        if( state == NotVisible)
        {
            _elementCycleState = NotVisible;
            elementDidDisappear();
            return;
        }
    }
    printf("ERROR (name %s): Current State is %i , next state is %i\n" , getElementName().c_str() , _elementCycleState , state );
    assert( false);
}

void GXElement::elementWillAppear()
{}

void GXElement::elementDidAppear()
{}

void GXElement::elementWillDisappear()
{

}

void GXElement::elementDidDisappear()
{

}

#endif /*ENABLE_ELEMENT_SELECTOR*/





