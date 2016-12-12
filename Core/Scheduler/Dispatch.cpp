//
//  Dispatch.cpp
//  Broadway_core
//
//  Created by Manuel Deneu on 02/07/2015.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//
#include <future>
#include "Dispatch.h"

#include "../Env_Variables.h"
#include "../Log/Log.h"

Dispatch* Dispatch::s_instance = nullptr;

Dispatch::Dispatch() :
 nextId        ( 1 ),
 _timersQueue  ( comparator ),
_shouldExit    ( false ),
_currentState  ( NotInitialized )
{
    className = ClassNames::Dispatch;
    setElementName( ClassNames::Dispatch );
    addTypeInfo( System );
}


Dispatch::~Dispatch()
{
    ScopedLock lock(_sync);
    _shouldExit = true;
    _wakeUp.notify_all();
    lock.unlock();

}


void Dispatch::stop()
{
    ScopedLock lock(_sync);
    _shouldExit = true;
    _wakeUp.notify_all();
//    lock.unlock();
    

}
void Dispatch::processNotification()
{
    auto msg = _msgQueue.front();
    if( msg)
    {
        // special case for selectors
        if( msg->getType() == Event_Selector)
        {
            const EventSelector *sel = static_cast<const EventSelector*>(msg);
            
            sel->target->performSelectorWithArguments(sel->selector, sel->arguments);
            
            delete msg;
        }
        else if(msg->getType() == Event_Function )
        {
            
        }
        else
        {
            for( auto && listeners : _listeners )
                listeners->eventReceived( msg );
            
            delete msg;
        }
    }
    
    _msgQueue.pop();
}

bool Dispatch::run()
{
    _mainThreadID = std::this_thread::get_id(); //noexcept
    ScopedLock lock(_sync); // pas noexcept
    
    changeStateTo( Running );
    while (!_shouldExit)
    {
        singleStep( lock );
    } // end while
    
    
    changeStateTo( NotRunning );
    removeAllNotifications();
    assert( _msgQueue.size() == 0 );

    return true;
}

bool Dispatch::runModalLoopForElement( Element *element , bool* shouldQuit)
{
    ScopedLock lock(_sync); // pas noexcept
    
    changeStateTo( RunningModal );
    
    while (*shouldQuit == false)
    {
        singleStep( lock );
    }

    lock.unlock();
    return true;
}

void Dispatch::singleStep( ScopedLock &lock)
{
    if( !_msgQueue.empty())
    {
        lock.unlock();
        processNotification();
        lock.lock();
        
    }
    if (!_timersQueue.empty())
    {
        auto firstInstance = _timersQueue.begin();
        Instance& instance = *firstInstance;
        auto now = Clock::now();
        
        if (now >= instance.next)
        {
            _timersQueue.erase( firstInstance );
            instance.running = true;
            
            const Chrono start;
            
            lock.unlock();
            instance.handler();
            lock.lock();
            
            const auto ellapsed = start.elapsedTimeInMs();
            const auto period = instance.period.count() ;
            
            if( period != 0 )
            {
                if(  ellapsed > period  )
                {
                    Log::log(" Task %llu \"%s\"takes too long! period %lli / real %lld  " ,
                             instance.id ,
                             instance._identifier.c_str(),
                             period ,
                             ellapsed
                             );
                    
                    if( instance._flexible )
                        instance.period = Duration( ellapsed );
                    else
                        Log::log("\t Timer not flexible, You should consider the option ");
                }
            }
            if (_shouldExit)
            {
                lock.unlock();
                return;
            }
            if (!instance.running)
            {
                // Running was set to false
                active.erase(instance.id);
            }
            else
            {
                instance.running = false;
                
                if (instance.period.count() > 0)
                {
                    instance.next = instance.next + instance.period;
                    _timersQueue.insert(instance);
                }
                else
                    active.erase(instance.id);
            }
        }
        else
        {
            // Wait until the timer is ready or a timer creation notifies
            if( _msgQueue.empty())
                _wakeUp.wait_until(lock, instance.next);
        }
    }
}


Dispatch::timer_id Dispatch::create(uint64_t msFromNow,
                                    uint64_t msPeriod,
                                    const std::function<void()> &handler ,
                                    const std::string &identifier
                                    )
{
    return createImpl(Instance(0,
                               Clock::now() + Duration(msFromNow), Duration(msPeriod),
                               handler), identifier);
}

Dispatch::timer_id Dispatch::create(uint64_t msFromNow,
                                    uint64_t msPeriod,
                                    std::function<void()>&& handler,
                                    const std::string &identifier
                                    )
{
    return createImpl(Instance(0,
                               Clock::now() + Duration(msFromNow), Duration(msPeriod),
                               std::move(handler)) , identifier );
}

Dispatch::timer_id Dispatch::createImpl(Instance&& item , const std::string &identifier)
{
    if( _sync.try_lock()) //noexcept
    {
        item._identifier = identifier;
    
        item.id = nextId++;
        auto iter = active.emplace(item.id, std::move(item)); // pas noexcept
        _timersQueue.insert(iter.first->second);              // pas noexcept 
        _wakeUp.notify_all();
        
        _sync.unlock();
        return item.id;
    }
    else
    {
        assert( false);
    }
    
    return  0;
}

bool Dispatch::destroy(timer_id id)
{
    ScopedLock lock(_sync);
    
    auto i = active.find(id);
    
    if (i == active.end())
        return false;
    
    else if (i->second.running)
    {
        // A callback is in progress for this Instance,
        // so flag it for deletion in the worker
        i->second.running = false;
    }
    else
    {
        _timersQueue.erase(std::ref(i->second));
        active.erase(i);
    }
    
    _wakeUp.notify_all();
    return true;
}

bool Dispatch::exists(timer_id id) 
{
    ScopedLock lock(_sync);
    return active.find(id) != active.end();
}

Dispatch::Instance *Dispatch::getTimer( timer_id id)
{

    ScopedLock lock(_sync);
    return &(active.find(id)->second);
    
}



#ifdef ENABLE_ELEMENT_SELECTOR

const Variant Dispatch::performAsync(Element*sender, Element* target ,const std::string &selector ,const VariantList  &arguments)
{
    pushNotification( new EventSelector(selector, target, arguments ,sender) );
    
    return Variant::null();
}
const Variant Dispatch::performSelectorWithArguments( const std::string &selector , const VariantList  &arguments , bool *validCall)
{
    if (validCall)
        *validCall = true;

    if( selector == "getActiveTimersCount")
        return Variant (  getActiveTimersCount() );

    if (validCall)
        *validCall = false;
    return Element::performSelectorWithArguments(selector, arguments , validCall );
}
#endif
