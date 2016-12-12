//
//  GXAnimation.h
//  Broadway_test
//
//  Created by Manuel Deneu on 17/10/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_test__GXAnimation__
#define __Broadway_test__GXAnimation__

#include <iostream>
#include <vector>

#include "GXElement.h"
#include "../Internal/Element.h"
#include "../Env_Variables.h"

typedef std::chrono::system_clock           AnimChrono;
typedef std::chrono::time_point<AnimChrono> AnimTime;
typedef std::chrono::duration<double>       AnimTimeMeasure;
typedef double                              AnimDuration; /* seconds */

class GXElement;

class GXAnimation : Element
{
    friend class GXElement;
    
public:
    
    typedef enum
    {
        Anim_None     =   0,        /* 0 No animation  */
        Anim_MoveTo   = ( 1 << 0),  /* 1 translation   */
        Anim_JumpTo   = ( 1 << 1),  /* 2 pos. jumps to */
        
        Anim_RotateTo = ( 1 << 2 ), /* 3 rotation      */
        
        
        Anim_FadeTo   = ( 1 << 3 )
        
    } AnimFlags;

    static GXAnimation* moveTo( const GXPoint &endPoint , AnimDuration seconds);
    static GXAnimation* wait( AnimDuration seconds);
    static GXAnimation* fadeTo( uint8_t val , AnimDuration seconds);
    static GXAnimation* sequence( std::vector<GXAnimation*> list );

    
    virtual ~GXAnimation();
    
    bool isStarted() const
    {
        return _started;
    }
    
    void desc()
    {
        GXAnimation *current = this;
        int count = 0;

        while (current != nullptr)
        {
            printf("\n Anim num %i" , count);

            current = current->_next;
            
            count++;
        }

    }
    
#ifdef ENABLE_ELEMENT_SELECTOR
    
    static const Variant staticSelector(const std::string& selector , const VariantList& arguments, bool *validCall);
    static void init()
    {
        Element::addStaticCallbackForClassName( staticSelector, ClassNames::GXAnimation );
    }
#endif
    
private: /* Accessed by and via GXElements only */
    
    GXAnimation( AnimDuration duration );
    
    bool prepareAnimationForElement( GXElement *element);
    bool processAnimation();
    
    void addAnimFlag( const AnimFlags anim)
    {
        _flags = ( AnimFlags ) ( _flags | anim );
    }
    
    bool hasAnimFlag( const AnimFlags anim)
    {
        return _flags & anim;
    }
    
    int hasFrames() const;
    
    bool start()
    {
        _startTime = AnimChrono::now();
        _started = true;
        

        
        printf("\n Computed num frames = %i \n" , _estimatedNumFrames );
        return  true;
    }
    bool ended() const
    {
        return _ended;
    }
    
    inline AnimDuration elapsedTime() const
    {
        const AnimTimeMeasure meas = AnimChrono::now() - _startTime;
        return meas.count();
    }
    
    /* *** */
    AnimFlags _flags;
    
    AnimDuration remainingTime();
    
    bool         _started;
    bool         _ended;
    
    AnimTime     _startTime;
    AnimDuration _duration;
    
    int _estimatedNumFrames;
    int _framesCounted;
    
    /* *** */
    //translation
    
    GXPoint _startPoint;
    GXPoint _destPoint;
    GXSize  _moveToSteps;
    
    /* *** */
    
    uint8_t _fadeTo;
    int     _fadeStep;
    
    
    GXElement *_attachedElement; // ref
    
    GXAnimation *_next;
    

};

#endif /* defined(__Broadway_test__GXAnimation__) */
