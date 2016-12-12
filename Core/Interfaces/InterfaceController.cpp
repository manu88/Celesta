//
//  GpioController.cpp
//  
//
//  Created by Manuel Deneu on 08/08/14.
//
//

#include <stdio.h>
#include <assert.h>

#include <sys/time.h>

#include <typeinfo>       // std::bad_cast
/* Adds Mouse-Devices*/
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>


/**/

#include "InterfaceController.h"

#include "../Internal/Element.h"
#include "../Log/Log.h"

#include "../Env_Variables.h"

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

// empty impl
void InterfaceControllerDelegate::inputChanged( const InterfaceEvent * )
{
    /* base class 'inputChanged' called */
    assert(false);
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */


InterfaceController::InterfaceController():
    Dispatch::Task     ( DEF_TASK_POLL , ClassNames::InterfaceController  ),
    _delegate          ( nullptr )
{
    className = ClassNames::InterfaceController;
    
    addTypeInfo( Hardware );
    addTypeInfo( Input );
    addTypeInfo( Output );
    
    if (! init())
    {
        Log::log("\n error while initializing gpio interface");
    }
}

InterfaceController::~InterfaceController()
{
    
    removeAllInputs();
    
    deInit();

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool InterfaceController::init()
{
    return GpioEvent::init();
}


void InterfaceController::deInit()
{
    return GpioEvent::deInit();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool InterfaceController::start() noexcept
{
    return true;
}

bool InterfaceController::stop() noexcept
{
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

GpioEvent* InterfaceController::addGpioInput(const unsigned int pinNumber  , GPioInputType typeOfInput )
{
    if ( !getGpioEventByPin( pinNumber ) )
    {
        
        GpioEvent *event = new GpioEvent(pinNumber , typeOfInput , this);
        _inputs.insert( event );

        return event;
    }


    return nullptr ;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
GpioEvent* InterfaceController::getGpioEventByID( Dispatch::element_id _id)
{
    for ( auto i : _inputs )
    {        
        if ( i->isGpioEvent() && ( i->getElementId() == _id) )
        {
            return  dynamic_cast< GpioEvent*>( i) ;
        }
        
    }
    
    return nullptr;
}
 */

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool InterfaceController::removeGpioInput(const unsigned int pinNumber)
{

    GpioEvent *eventToRemove  = nullptr;
    
    if ( ( eventToRemove = getGpioEventByPin( pinNumber ) ) != nullptr )
    {
        delete eventToRemove;
        
        _inputs.erase( eventToRemove );
        
        return true;
    }
    
    return false;
}

void InterfaceController::removeAllInputs()
{

    for ( auto i : _inputs )
    {
        i->cleanup();
        delete i;
    }
    
    _inputs.clear();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

GpioEvent* InterfaceController::getGpioEventByPin( const unsigned int pin) const noexcept
{
    for ( auto i : _inputs )
    {
        if ( i->isGpioEvent() )
        {
            GpioEvent * gpio = nullptr;
            
            try { gpio = dynamic_cast< GpioEvent*>( i ); }
            
            catch( std::bad_cast& bc )
            {
                printf("\n bad cast %s " , bc.what() );
                return  nullptr;
            }
            
            if ( gpio->getPin() == pin)
                return gpio;
        }
    }
    
    return nullptr;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void InterfaceController::listActivesInput()
{

}

void InterfaceController::sendGpo(const unsigned int pinNumber , const GpioState state)
{
    if (getGpioEventByPin( pinNumber))
        Log::log("WARNING : trying to use a GPIO Pin registered as input for output... DANGEROUS!");
    else
        GpioEvent::setGpio(pinNumber, state);
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

SerialEvent* InterfaceController::addSerial( const std::string &port)
{

    SerialEvent* event = new SerialEvent( port );

    return addSerial(event);
}

SerialEvent* InterfaceController::addSerial( SerialEvent *event)
{
    if( event->openPort() )
        _inputs.insert( event );
    
    return event;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

SpiEvent* InterfaceController::addSpi( SpiChipSelect cs)
{
    
    SpiEvent *event = new SpiEvent( cs  , this);
    
    _inputs.insert( event );
    
    
    return event;
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

SerialEvent* InterfaceController::getSerialEventByPort( const std::string &port)
{
    for ( auto i : _inputs)
    {
        if (i->isSerialEvent() )
        {
            SerialEvent *event = dynamic_cast<SerialEvent*>( i );
            if ( event->getPort() == port )
                return event;
        }
    }
    
    return nullptr;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

CanEvent* InterfaceController::addCanConnexion( const std::string &interface)
{

    
    CanEvent* event = new CanEvent( interface  , this);
    _inputs.insert( event );
    
    event->connect();
    
    return event;
}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
 
    Loop : read registered inputs pins and call the delegate on changes
 
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
void InterfaceController::read()
{

    for ( auto i : _inputs )
    {

        if ( i->changed() )//&&  _delegate->delegateReadyForController( this )  )
        {
            _delegate->inputChanged ( i );
        }
    }
    
}


#ifdef ENABLE_ELEMENT_SELECTOR
const Variant InterfaceController::performSelectorWithArguments( const std::string &selector , const VariantList  &arguments, bool *validCall )
{
    if( validCall)
        *validCall = true;
    
    /**/
    
    if(selector=="addGPIO")
    {
        //( pin , mode)
        
        const unsigned int pin  = static_cast<unsigned int>( arguments.at(0).getInt() );
        const int mode = arguments.at(1).getInt();
        
        const bool ret = addGpioInput(pin,(GPioInputType) mode) != nullptr;
        
        return Variant( ret );
    }
    
    else if( selector =="writeGPIO")
    {
        const unsigned int pin  = static_cast<unsigned int>( arguments.at(0).getInt() );
        const int value = arguments.at(1).getInt();
        sendGpo(pin,(GpioState) value) ;

        return  Variant::null();
    }
    

    
    /**/

    if( validCall)
        *validCall = false;
    
    return Element::performSelectorWithArguments(selector, arguments, validCall);
}
#endif






