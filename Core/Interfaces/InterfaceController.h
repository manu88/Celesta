//
//  GpioController.h
//  
//
//  Created by Manuel Deneu on 08/08/14.
//
//

#ifndef ____GpioController__
#define ____GpioController__

#include <iostream>
#include <unordered_set>
#include <string.h>

#include "../Internal/Thread.h"
#include "../Scheduler/Event.h"

#include "../Internal/AbstractController.h"
#include "../Internal/AbstractDelegate.h"


#include "SerialInterface.h"
#include "InterfaceEvent.h"
#include "GpioInterface.h"
#include "CanEvent.h"


#include "../Scheduler/Dispatch.h"

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class InterfaceControllerDelegate : public virtual AbstractDelegate
{
    friend class InterfaceController;
    
public:
    virtual ~InterfaceControllerDelegate() {}
    
protected:
    InterfaceControllerDelegate(){}
    
    virtual void inputChanged( const InterfaceEvent *event ) = 0;
};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class InterfaceController : public Element,
                            public Dispatch::Task
{
public:

    enum { DEF_TASK_POLL = 500 };
    
    InterfaceController();
    virtual ~InterfaceController();
    
    void setDelegate( InterfaceControllerDelegate* delegate) noexcept
    {
        _delegate = delegate;
    }
    
    bool init();
    void deInit();
    
    bool start() noexcept;
    bool stop() noexcept;
    
    inline void doTask()
    {
        read();
    }

    /* GPIO part */
    
    GpioEvent* addGpioInput(const unsigned int pinNumber , GPioInputType typeOfInput);
    bool removeGpioInput(const unsigned int pinNumber);
    
    //GpioEvent* getGpioEventByID( Dispatch::element_id _id);
    
    void removeAllInputs();
    
    void sendGpo(const unsigned int pinNumber , const GpioState state);
    
    GpioEvent* getGpioEventByPin( const unsigned int pin) const noexcept;

    /* Serial part */
    
    SerialEvent *addSerial( const std::string &port);
    SerialEvent *addSerial( SerialEvent *event);
    SerialEvent *getSerialEventByPort( const std::string &port);
    
    bool addEvent( InterfaceEvent* event)
    {
        _inputs.insert( event);
        return true;
    }
    

    
    
    /* Can connections */
    
    CanEvent *addCanConnexion( const std::string &interface);
    
    /* SPI */
    
    SpiEvent *addSpi( SpiChipSelect cs);


    
    void listActivesInput();
    
#ifdef ENABLE_ELEMENT_SELECTOR
    const Variant performSelectorWithArguments( const std::string &selector , const VariantList  &arguments, bool *validCall = nullptr ) ;
#endif
    
private:
    void read();

    InterfaceControllerDelegate           *_delegate;
    std::unordered_set< InterfaceEvent* >  _inputs;
    

};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

#endif /* defined(____GpioController__) */
