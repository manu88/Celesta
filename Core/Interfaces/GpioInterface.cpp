//
//  GpioInterface.cpp
//  Broadway_core
//
//  Created by Manuel Deneu on 15/12/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#include <sys/time.h>
#include "../Scheduler/Dispatch.h"
#include "GpioInterface.h"
#include "../Scheduler/Chrono.h"

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

SpiEvent::SpiEvent( SpiChipSelect cs , Element *sender):
InterfaceEvent( Event_SPI  , sender),
_impl( cs )
{
    
}

SpiEvent::~SpiEvent()
{
    
}

bool SpiEvent::changed()
{
    return false;
}

void SpiEvent::cleanup()
{
    
}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

GpioEvent::GpioEvent( unsigned int pinToUse , GPioInputType typeOfInput , Element *sender) :
InterfaceEvent( Event_Gpio , sender ),
_impl( pinToUse , typeOfInput ),

_pin               ( pinToUse  ),
_state             ( undefined ),
_lastState         ( undefined ),

_debounceDelay     ( DEF_DEBOUNCE_DELAY        ), // default val
_lastDebounceTime (0          )
{
    className = "GpioEvent";
    
    //setElementName("GpioEvent");
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

GpioEvent::~GpioEvent()
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ void GpioEvent::setGpio( const unsigned int pin , const GpioState state)
{
    GpioPlateformImplementation::setGpio( pin, state);
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool GpioEvent::changed()
{
    GpioState newState = _impl.read();

    
    if (newState != _lastState)
    {
        _lastDebounceTime = static_cast<long>( Dispatch::getTimeInMs() );
    }
    
    if ( ( Dispatch::getTimeInMs() - _lastDebounceTime) > _debounceDelay)
    {
        if (newState != _state)
        {
            _state = newState;
            _lastState = newState;
            /*
             if (    (               m_transition == Changed )
             || ( state == 0 && m_transition == Falling )
             || ( state == 1 && m_transition == Rising  )
             )
             //callSelector();
             */
            return true;
        }
    }
    
    _lastState = newState;
    
    return false;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

GpioState GpioEvent::read()
{
    return _state;
}




