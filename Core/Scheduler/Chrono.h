//
//  Scheduler.h
//  
//
//  Created by Manuel Deneu on 11/08/14.
//
//

#ifndef ____Scheduler__
#define ____Scheduler__

#include <iostream>
#include <unordered_map>

//#include <sys/time.h>
//#include <chrono>
//#include <mutex>
//#include <condition_variable>

#include "Event.h"
#include "../Internal/Object.h"
#include "../Internal/Thread.h"
#include "../Internal/AbstractController.h"
#include "../Internal/AbstractDelegate.h"

#include "Timecode.h"
#include "Event.h"

#include "../Env_Variables.h"

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class Chrono : public Element
{
public:
    
    Chrono()
    {
        setElementName( ClassNames::Chrono + std::to_string( getElementId()) );
        reset();
        
        className = ClassNames::Chrono;
    }
    
    virtual ~Chrono()
    {}
    
    void reset()
    {
        _start       = std::chrono::high_resolution_clock::now();
        _lastMeasure = std::chrono::high_resolution_clock::now();
    }
    
    inline long long  elapsedTimeInMs() const
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>( _lastMeasure - _start ).count();
    }
    
    inline long long timeSinceLastMeasure() const
    {
        const auto ret =std::chrono::duration_cast<std::chrono::milliseconds> ( std::chrono::high_resolution_clock::now() - _lastMeasure ).count();
        
        _lastMeasure = std::chrono::high_resolution_clock::now();
        return ret;
    }
    
#ifdef ENABLE_ELEMENT_SELECTOR

    const Variant performSelectorWithArguments( const std::string &selector , const VariantList  &arguments, bool *validCall = nullptr )
    {
        if( validCall)
        {
            *validCall = true;
        }
        
        if( selector == "reset")
        {
            reset();
            return Variant::null();
        }
        
        else if ( selector == "elapsedTimeInMs")
        {
            return Variant( elapsedTimeInMs() );
        }
        
        else if ( selector == "timeSinceLastMeasure")
        {
            return Variant( timeSinceLastMeasure() );
        }
        
        if( validCall)
        {
            *validCall = false;
        }
        
        return Element::performSelectorWithArguments( selector, arguments, validCall );
    }
#endif
    
private:
    
    std::chrono::high_resolution_clock::time_point _start;
    mutable std::chrono::high_resolution_clock::time_point _lastMeasure;
    
};




#endif /* defined(____Scheduler__) */
