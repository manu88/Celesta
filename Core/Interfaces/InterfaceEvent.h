//
//  InterfaceEvent.h
//  Broadway_core
//
//  Created by Manuel Deneu on 15/12/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_core__InterfaceEvent__
#define __Broadway_core__InterfaceEvent__

#include <iostream>

#include "../Internal/Element.h"

#include "../Scheduler/Event.h"

typedef enum
{
    Event_Gpio,   // GPIO HIGH/LOW state
    Event_I2C,    // I2C device
    Event_SPI,    // SPI device
    Event_Serial,  // Serial Interface
    Event_CAN,
    Event_Mouse,
    Event_Keyboard,
    Event_Touch
    
} InterfaceEventType;


class InterfaceEvent : public Event
{
    friend class InterfaceController;
    
public:
    virtual ~InterfaceEvent();
    
    // subclasses need to overide this
    virtual bool changed() = 0;

    
    /* **** */
    
    InterfaceEventType getType() const noexcept
    {
        return _type;
    }
    
    /* **** */
    
    bool isGpioEvent() const noexcept
    {
        return _type == Event_Gpio;
    }
    
    bool isSerialEvent() const noexcept
    {
        return _type == Event_Serial;
    }
    
    bool isCanEvent() const noexcept
    {
        return _type == Event_CAN;
    }
    
    bool isSpiEvent() const noexcept
    {
        return _type == Event_SPI;
    }
   
protected:
    // this method will be called before destruction by InterfaceController instance
    virtual void cleanup() = 0;
    
    InterfaceEvent( InterfaceEventType type , Element *sender);
    
private:
    
    InterfaceEventType _type;
    
};


#endif /* defined(__Broadway_core__InterfaceEvent__) */
