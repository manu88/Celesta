//
//  GXAnimation.cpp
//  Broadway_test
//
//  Created by Manuel Deneu on 17/10/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//
#include "../Env_Variables.h"
#include "GXAnimation.h"
#include "DisplayController.h"
#include "../Scheduler/Dispatch.h"

GXAnimation::GXAnimation( AnimDuration duration  ):
_flags   ( Anim_None ),
_started ( false ),
_ended   ( false  ),

_duration( duration ),

_estimatedNumFrames     ( -1 ),
_framesCounted          ( 0  ),
_attachedElement ( nullptr ),
_next            ( nullptr )
{
    className = ClassNames::GXAnimation;
    
    
    setElementName( ClassNames::GXAnimation );
    // predicted num frames
    _estimatedNumFrames = floor( _duration * static_cast<double>( DisplayController::getInstance()->getCurrentFrameRate() ) );
    

}

GXAnimation::~GXAnimation()
{
/*    if( _next )
        delete _next;*/
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ GXAnimation* GXAnimation::moveTo( const GXPoint &endPoint , AnimDuration seconds)
{
    GXAnimation *anim = new GXAnimation( seconds );
    anim->setElementName("GXAnimationMoveTo");
    anim->addAnimFlag( Anim_MoveTo );
    anim->_destPoint = endPoint;
    
    return anim;
}

/*static*/ GXAnimation* GXAnimation::wait( AnimDuration seconds)
{
    GXAnimation *anim = new GXAnimation( seconds );
    anim->setElementName("GXAnimationWait");
    anim->addAnimFlag( Anim_None );
    
    return anim;
}

/*static*/ GXAnimation* GXAnimation::fadeTo( uint8_t val , AnimDuration seconds)
{
    GXAnimation *anim = new GXAnimation( seconds );
    anim->setElementName("GXAnimationFadeTo");
    anim->addAnimFlag( Anim_FadeTo );
    anim->_fadeTo = val;
    
    return anim;
}

/*static*/ GXAnimation* GXAnimation::sequence( std::vector<GXAnimation*> list )
{
    printf("\n Animation list size %li" , list.size() );
    GXAnimation *prev = list.at( 0 );
    /*
     for (auto it = list.begin() + 1; it != list.end(); it++)
     {
     prev->_next = *it;
     prev = *it;
     
     }
     */
    
    list.at(0)->_next = list.at(1);
    list.at(1)->_next = list.at(2);
    
    return list.at(0);
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

int GXAnimation::hasFrames() const
{
    return 1;
}

bool GXAnimation::prepareAnimationForElement( GXElement *element)
{
    _attachedElement = element;
    
    _startPoint = _attachedElement->getBounds().origin;
    
    printf("\n prepare anim");
    
    if( hasAnimFlag(Anim_MoveTo))
    {
        printf("\n has Move to flag");
        
        float dX = 0.0f;
        float dY = 0.0f;
        
        if( _estimatedNumFrames != 0)
        {
            dX = (_destPoint.x - _startPoint.x) / _estimatedNumFrames;
            dY = (_destPoint.y - _startPoint.y) / _estimatedNumFrames;
        }
        else
            printf("\n _estimatedNumFrames = 0");
        
        printf("\n dx = %f , dy = %f " , dX ,dY);
        _moveToSteps = makeSize( dX, dY );
        

    }
    if( hasAnimFlag( Anim_FadeTo ))
    {
        printf("\n has fade to flag");
        const uint8_t current = element->_alpha;
        const uint8_t to      = _fadeTo;
        
        const int dir = to > current? 1 : -1; // 1 = Up
        const int len = (to-current);
        
        float dA = float(len) / _estimatedNumFrames;
        
        _fadeStep = dA;
        
        if( dA < 1)
            _fadeStep = dir;
        
        
        
        printf(" dir = %i , len = %i delta Alpha = %f \n" ,dir, len ,dA);
        
    }
    
    return true;
}

bool GXAnimation::processAnimation()
{
    
    if( hasAnimFlag(Anim_MoveTo))
    {
        _attachedElement->moveOf( _moveToSteps.height , _moveToSteps.width );
    }
    
    if( hasAnimFlag( Anim_FadeTo ) )
    {
        int val =_attachedElement->_alpha + _fadeStep;
        
        if(
              ( (_fadeStep >0) && (val > _fadeTo ) )
           || ( (_fadeStep <0) && (val < _fadeTo ) )
           )
           val = _fadeTo;
        

        
        
        _attachedElement->setAlpha( val );
        printf("alpha %i \n" , val );

   
    }

    
    return true;
}

AnimDuration GXAnimation::remainingTime()
{
    const AnimDuration diff = _duration - elapsedTime();
        
    if( diff <=0 )
    {
        printf("\n stop anim %i \n" , _framesCounted );
        _started = false;
        _ended   = true;
        
        if( _next == nullptr )
            Dispatch::getInstance()->pushNotification( _attachedElement->_animCompletionEvent );
        
        return 0;
    }
    
    return diff;
}

const Variant GXAnimation::staticSelector(const std::string& selector , const VariantList& arguments, bool *validCall)
{
    if( validCall )
        *validCall = true;
    
    if( selector == "GXAnimationMoveTo")
    {
        GXAnimation * anim =  GXAnimation::moveTo( makePoint( arguments.at(0).getInt(),
                                                             arguments.at(1).getInt()
                                                             ),
                                                  arguments.at(2).getFloat()
                                                  );
        
        return  Variant ( anim->getElementId() );
    }
    
    else if( selector == "GXAnimationWait")
    {
        GXAnimation * anim =  GXAnimation::wait( arguments.at(0).getFloat() );
        
        return  Variant ( anim->getElementId() );
    }
    
    else if( selector == "GXAnimationFadeTo")
    {
        GXAnimation * anim =  GXAnimation::fadeTo( arguments.at(0).getInt() , arguments.at(1).getFloat() );
        
        return  Variant ( anim->getElementId() );
    }
    
    else if ( selector == "GXAnimationSequence")
    {
        std::vector<GXAnimation*> animList;
        for ( const Variant &e : arguments )
        {
            animList.push_back( reinterpret_cast<GXAnimation*>( Element::getElementById( e.getInt() )  ) );
            printf("\n add anim to list");
        }
        
        printf("\n Anim List size = %li" , animList.size() );
        return Variant( GXAnimation::sequence( animList )->getElementId() );
        
    }
    
    if( validCall )
        *validCall = false;
    
    return Element::staticSelector(selector, arguments, validCall );
}


