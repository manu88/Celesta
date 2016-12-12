//
//  Event.h
//  
//
//  Created by Manuel Deneu on 12/08/14.
//
//

#ifndef ____Event__
#define ____Event__

#include <iostream>

#include "../Internal/Element.h"

/* **** **** **** **** **** **** */

typedef enum
{
    Event_Base     = 0,   //
    Event_Timer    = 1,   // implemented by TimedEvent
    
    Event_Timeout  = 2,   // implemented by TimeoutEvent
    
    Event_Selector = 3,
    Event_Message  = 4, // simple text message
    Event_Function = 5,
    Event_Input   = 100, // implemented by GpioEvent
    
    Event_Network = 200,
    Event_OSC     = 201,
    
    Event_GET     = 230,
    
    Event_Video   = 300,   // implemented by GXVideo
    Event_Display = 320,
    Event_Action  = 400,   // implemented in GXUI system
    
    Event_AppBase     = 500,
    Event_FileManager = 510,
    

    
} EventType;


/* **** **** **** **** **** **** */


//! Event
/********************************************//**
*
*   Base class for any type of Event with a callback.
*   You have to register a Selector witch will be performed when the event is fired.
*                                                   
*   \sa GpioEvent OscEvent
*
*   \relates Transition
***********************************************/
class Event : public virtual Object
{
public:
    Event( EventType type , Element* sender );
    
    virtual ~Event();
    
    /**/
    
    Element* getSender() const
    {
        return _sender;
    }
    
    const EventType &getType() const noexcept
    {
        return _type;
    }
    
    bool isTimer() const noexcept
    {
        return _type == Event_Timer;
    }
    
    bool isVideo() const noexcept
    {
        return _type == Event_Video;
    }
    
    bool isInput() const noexcept
    {
        return _type == Event_Input;
    }
    
    bool isAction() const noexcept
    {
        return _type == Event_Action;
    }
    
    bool isOsc() const noexcept
    {
        return _type == Event_OSC;
    }
    
    bool isMessage() const noexcept
    {
        return  _type == Event_Message;
    }
    
    /* Safe cast */
    
    template<typename T>
    static const T cast( const Event* event)
    {
        return static_cast<T>(event);
    }

protected:
    

    EventType _type;
    Element *_sender;
    
};

/* **** **** **** **** **** **** **** **** **** **** */

class EventFunction : public Event
{
    typedef std::function<void(Element*, const VariantList &)> Signature;
public:
    EventFunction( Signature &_function, Element* _target,   const VariantList &_arguments ,Element *sender):
    Event(Event_Function , sender),
    //function (std::bind( _function , target ) ),
    target( _target),
    arguments( _arguments )
    {
        
    }
    
//    std::mem_fn  function;
    Element     *target;
    VariantList arguments;
    /*
    void call()
    {
        function( _sender , arguments );
    }
     */
    
};

/* **** **** **** **** **** **** **** **** **** **** */

class EventSelector : public Event
{
public:
    EventSelector( const std::string &_selector , Element* _target , const VariantList &_arguments , Element *sender):
    Event( Event_Selector  , sender),
    selector( _selector ),
    target ( _target ),
    arguments ( _arguments)
    {
        s_count++;
    }
    
    ~EventSelector()
    {
        s_count--;
    }
    
    std::string selector;
    Element     *target;
    VariantList arguments;
    
private:
    static int s_count;
};

/* **** **** **** **** **** **** **** **** **** **** */

class EventMessage : public Event
{
public:
    EventMessage( const std::string &msg , Element* sender ):
    Event(Event_Message, sender ),
    message(msg)
    {
        s_count++;
    }
    
    ~EventMessage()
    {}
    
    std::string message;
    
private:
    
    static int s_count;
    
};

#endif /* defined(____Event__) */
