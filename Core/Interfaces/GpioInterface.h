//
//  GpioInterface.h
//  Broadway_core
//
//  Created by Manuel Deneu on 15/12/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_core__GpioInterface__
#define __Broadway_core__GpioInterface__

#include "InterfaceEvent.h"

#include "../Internal/Element.h"

#include "Impl/GpioPlateformImpl.h"






/* **** **** **** **** **** **** **** **** **** **** **** */

class SpiEvent : public InterfaceEvent
{
public:
    
    SpiEvent( SpiChipSelect cs , Element *sender );
    ~SpiEvent();
    
    void setCsPolarity( GpioState state)
    {
        _impl.setCsPolarity( state );
    }
    
    static void setBitOrder     ( SpiBitOrder order)
    {
        SpiPlateformImplementation::setBitOrder( order );
    }
    
    static void setDataMode     ( SpiDataMode mode)
    {
        SpiPlateformImplementation::setDataMode( mode );
    }
    
    static void setClockDivider ( SpiClockDivider divider)
    {
        SpiPlateformImplementation::setClockDivider( divider );
    }
    
    bool changed();

    
    inline uint8_t writeRead( const uint8_t &data )
    {
        return _impl.writeRead( data );
    }
    
    inline void writeReadMult( const uint8_t *send , uint8_t *rec , uint32_t length)
    {
        return _impl.writeReadMult( send , rec , length);
    }
    
    
private:
    void cleanup();
    
    SpiPlateformImplementation _impl;

};

/* **** **** **** **** **** **** **** **** **** **** **** */

class GpioEvent :  public InterfaceEvent
{
public :
    
    enum { DEF_DEBOUNCE_DELAY = 10};
    
    GpioEvent( unsigned int pinToUse , GPioInputType typeOfInput , Element *sender);
    
    ~GpioEvent();
    

    
    
    static bool init()
    {
        return GpioPlateformImplementation::init();
    }
    
    static void deInit()
    {
        GpioPlateformImplementation::deInit();
    }
    

    
    bool changed();
    GpioState read();
    
    
    static void setGpio( const unsigned int pin , const GpioState state);
    
    void setDebounceTime( long delay ) noexcept
    {
        _debounceDelay = delay;
    }
    
    long getDebounceTime() const noexcept
    {
        return _debounceDelay;
    }
    
    const GpioState &getState() const noexcept
    {
        return _state;
    }
    
    unsigned int getPin() const noexcept
    {
        return _pin;
    }
    

    
private:
    void cleanup()
    {
    }
    
    // actual plateform specific implementation
    GpioPlateformImplementation _impl;
    
    unsigned int _pin;
    
    GpioState _state;
    GpioState _lastState;

    long      _debounceDelay;    
    long      _lastDebounceTime;

    
};



#endif /* defined(__Broadway_core__GpioInterface__) */
