//
//  Dispatch.h
//  Broadway_core
//
//  Created by Manuel Deneu on 02/07/2015.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_core__Dispatch__
#define __Broadway_core__Dispatch__

#include <stdio.h>
#include <unordered_map>
#include <queue>
#include <vector>

#include "../Internal/Object.h"
#include "../Internal/AbstractDelegate.h"
#include "../Internal/AbstractController.h"

#include "../Log/Log.h"
#include "Chrono.h"

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class EventListener
{
public:
    virtual void eventReceived( const Event *event) = 0;
    
protected:
    EventListener(){}
    virtual ~EventListener(){}
    

};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class Dispatch : public Element
{
public:
    
    typedef enum
    {
        NotInitialized = -1,
        NotRunning     = 0,
        Running        = 1,
        RunningModal   = 2,
    } State;
    
    typedef uint64_t timer_id;
    typedef std::function<void()> handler_type;
    
    static Dispatch* getInstance()
    {
        if (s_instance == nullptr)
            s_instance = new Dispatch();
        
        return s_instance;
    }

    static void dispose()
    {
        if ( s_instance != nullptr)
            delete s_instance;
        
        s_instance = nullptr;
    }
    
    bool run();
    void stop();
    
    bool runModalLoopForElement( Element *element , bool* shouldQuit);
    
    /* **** TIMERS **** */
    
    timer_id create(uint64_t when, uint64_t period, const handler_type& handler , const std::string &identifier);
    timer_id create(uint64_t when, uint64_t period, handler_type&& handler , const std::string &identifier);
    
    bool destroy(timer_id id);
    bool exists(timer_id id);
    
    // bug
    inline bool timerSetFlexible( timer_id id )
    {
        if (exists( id ))
        {
            ScopedLock lock(_sync);
            getTimer( id )->_flexible = true;
            return true;
        }
        return false;
    }
    
    inline const std::string timerGetIdentifier( timer_id id)
    {
        if( exists( id))
        {
//            ScopedLock lock( _sync );
            return getTimer( id)->_identifier;
        }
        
        return std::string();
        
    }

    
    unsigned long getActiveTimersCount() const
    {
        return active.size();
    }

    /* **** MESSAGES **** */

    enum { Max_Messages_Inqueue = 10000 };
    
    void pushNotification( Event *event )
    {
        if (!event)
            return;

        ScopedLock lock(_sync);

        if( _msgQueue.size() >= Max_Messages_Inqueue )
        {
            Log::log("\n MAX MSG %i -> Queue size = %lu \n" , Max_Messages_Inqueue, _msgQueue.size() );
            removeAllNotifications();
        }

        _msgQueue.push( event);

        _wakeUp.notify_all();
        

    }

//    bool performAsync(Element* sender , )

    void addListener( EventListener *listener)
    {
        _listeners.push_back( listener );
    }
    
    void removeAllNotifications()
    {
        
        while (!_msgQueue.empty())
        {
            auto msg = _msgQueue.front();
            if( msg)
                delete msg;
            
            _msgQueue.pop();
        }
        
    }
    
    /* **** TASKS **** */

    class Task
    {
    public:
        
        friend class Dispatch;
        
        virtual ~Task(){}
        
        virtual void doTask() = 0;
        
        bool stop()
        {
            return Dispatch::getInstance()->removeTask( this );
        }
        
    protected:
        Task ( uint64_t period , const std::string &identifier ):
        _period  ( period ),
        _timerId ( 0 ),
        _identifier( identifier )
        {}
        
    private:
        uint64_t _period;
        timer_id _timerId;
        
        std::string _identifier;

    };
    
    void add( Task *task , uint64_t delayAtStart = 0)
    {
        task->_timerId =  create(delayAtStart, task->_period, std::bind(&Dispatch::Task::doTask, task) , task->_identifier );
        
        _runningTasks.push_back( task );
        
    }
    
    bool removeTask( Task *task)
    {
        if( exists( task->_timerId ) )
        {
            destroy( task->_timerId );
            _runningTasks.remove( task );
            return true;
        }
        
        return false;
    }
    
    const std::list<Task*> &getRunningTasks() const
    {
        return _runningTasks;
    }
    
    /* **** TimeOUT Event **** */
    
    class TimeoutEvent :  public Event , public Element
    {
        friend class Dispatch;
    public:
        
        TimeoutEvent( uint64_t timeToWait ) :
        Event       ( Event_Timeout ,this ),
        _timerId    ( 0 ),
        _timeToWait ( timeToWait )
        {}
        
        TimeoutEvent( const Timecode &tc):
        TimeoutEvent( tc.getInMs() )
        {}
        
        virtual ~TimeoutEvent()
        {
            cancel();
        }
        
        bool cancel()
        {
            if( Dispatch::getInstance()->destroy( _timerId ) )
            {
                _timerId = 0;
                return true;
            }
            
            return false;
        }
        
        bool schedule( const Timecode &timeToWait = 0)
        {
            if( _timerId != (timer_id) -1)
                return false;
            
            if( timeToWait != 0)
                _timeToWait = timeToWait.getInMs();
            
            
            _timerId = Dispatch::getInstance()->create( _timeToWait , 0, std::bind(&Dispatch::TimeoutEvent::onTime, this ) , getElementName() );
            return true;
        }
        
        
        inline Timecode getTimeToWait() const
        {
            return Timecode(0,0,0,_timeToWait );
        }
        
        inline Timecode getRemainingTime() const
        {
            
            if( Dispatch::getInstance()->exists(_timerId) )
            {
                const Dispatch::Instance *timer = Dispatch::getInstance()->getTimer(_timerId);
                
                const auto dt =  std::chrono::duration_cast<std::chrono::milliseconds>(   timer->next - Clock::now() ).count();
                
                const unsigned long ms =dt>=0? static_cast<unsigned long>( dt) : 0;
                return Timecode( 0,0,0, ms);
            }

            return Timecode();
        }
        
        inline Timecode getCurrentTC() const
        {
            return getTimeToWait()-getRemainingTime();
        }
        
        
    private:
        
        void onTime()
        {
            Dispatch::getInstance()->pushNotification(  this  );
            
            cancel();
        }
        
    private:
        timer_id _timerId;
        uint64_t _timeToWait;
    };
    
    static double getTimeInMs()
    {
        struct timeval t;
        
        gettimeofday( &t, 0 );
        
        return ((double)t.tv_sec*1000.) + ((double)t.tv_usec / 1000.);
    }
    
    
    /* **** Controllers **** */
    
    void add( AbstractController *task )
    {
        task->start();
    }
    
    void setAsMainThread()
    {
        _mainThreadID = std::this_thread::get_id();
    }
    
    static bool isTheMainThread()
    {
        return getInstance()->_mainThreadID == std::this_thread::get_id();
    }
    
    
#ifdef ENABLE_ELEMENT_SELECTOR
    
    const Variant performAsync( Element*sender , Element* target ,
                                const std::string &selector ,
                                const VariantList  &arguments );

    
    const Variant performSelectorWithArguments( const std::string &selector , const VariantList  &arguments , bool *validCall = nullptr) ;
#endif
    
private:
    
    std::mutex              _sync;
    std::condition_variable _wakeUp;
    
    typedef std::chrono::steady_clock Clock;
    typedef std::chrono::time_point<Clock> Timestamp;
    typedef std::chrono::milliseconds Duration;
    
    /* **** **** **** **** **** **** **** **** **** **** **** */
    /* **** **** **** **** **** **** **** **** **** **** **** */
    
    struct Instance
    {
        enum { FlexibilityDefault = true };
        
        Instance(timer_id _id = 0) :
        id(_id),
        running(false),
        _timeTaken( 0 ),
        _flexible   ( FlexibilityDefault )
        {
        }
        
        template<typename Tfunction>
        Instance(timer_id _id, Timestamp _next, Duration _period, Tfunction&& _handler) noexcept :
        id(_id),
        next(_next),
        period(_period),
        handler(std::forward<Tfunction>(_handler)),
        running(false),
        _timeTaken( 0 ),
        _flexible   ( FlexibilityDefault )
        {
        }
        
        Instance(Instance const& r) = delete;
        
        Instance(Instance&& r) noexcept :
        id          ( r.id),
        next        ( r.next),
        period      ( r.period ),
        handler     ( std::move(r.handler) ),
        running     ( r.running ),
        _timeTaken  ( r._timeTaken ),
        _flexible   ( r._flexible ),
        _identifier ( r._identifier )
        {
        }
        
        Instance& operator=(Instance const& r) = delete;
        
        Instance& operator=(Instance&& r)
        {
            if (this != &r)
            {
                id          = r.id;
                next        = r.next;
                period      = r.period;
                handler     = std::move(r.handler);
                running     = r.running;
                _timeTaken  = r._timeTaken;
                _flexible   = r._flexible;
                _identifier = r._identifier;
            }
            return *this;
        }
        
        timer_id id;
        Timestamp next;
        Duration period;
        handler_type handler;
        bool running;
        
        Duration    _timeTaken;
        bool        _flexible;
        std::string _identifier;
        
    };
    
    /* **** **** **** **** **** **** **** **** **** **** **** */
    /* **** **** **** **** **** **** **** **** **** **** **** */
    
    typedef std::unordered_map<timer_id, Instance> InstanceMap;
    timer_id nextId;
    InstanceMap active;
    
    Instance *getTimer( timer_id id);

    
    // Comparison functor to sort the timer "queue" by Instance::next
    struct NextActiveComparator
    {
        bool operator()(const Instance &a, const Instance &b) const
        {
            return a.next < b.next;
        }
    };
    NextActiveComparator comparator;
    
    // Queue is a set of references to Instance objects, sorted by next
    typedef std::reference_wrapper<Instance> QueueValue;
    typedef std::multiset<QueueValue, NextActiveComparator> Queue;

    
    // Thread and exit flag


private:
    void changeStateTo( Dispatch::State state)
    {
        if (state != _currentState )
        {
            _currentState = state;
            updateValueForKey(ElementAttributes::State, Variant( _currentState ));
            
        }
    }
    
    void singleStep( ScopedLock &lock);
    // no lock!!
    void processNotification();
    
    timer_id createImpl(Instance&& item , const std::string &identifier);
    
    Queue               _timersQueue;
    std::queue<Event *>  _msgQueue;
    
    std::vector< EventListener* > _listeners;
    
    std::list<Task*> _runningTasks;
    Dispatch();
    virtual ~Dispatch();
    
    bool _shouldExit;
    std::thread::id _mainThreadID;
    
    State _currentState;
    
    static Dispatch *s_instance;
};

#endif /* defined(__Broadway_core__Dispatch__) */
