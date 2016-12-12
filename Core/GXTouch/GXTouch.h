//
//  GXTouch.hpp
//  JSCenter
//
//  Created by Manuel Deneu on 23/11/2015.
//  Copyright © 2015 Manuel Deneu. All rights reserved.
//

#ifndef GXTouch_hpp
#define GXTouch_hpp

#include <stdio.h>

#include "../Internal/Element.h"
#include "../Scheduler/Dispatch.h"

#include "Peripherals.h"
#include "../GXDataType/GXGeometry.h"

#include "../Internal/Thread.h"


class GXTouchDelegate
{
public:
    virtual ~GXTouchDelegate()
    {}
    virtual bool toucheBegan( const std::list<GXTouch> &touches );
    virtual bool toucheMoved( const std::list<GXTouch> &touches );
    virtual bool toucheEnded( const std::list<GXTouch> &touches );
};

class TouchThread : public Thread
{
public:
    TouchThread():
    Thread("TouchThread")
    {}
    
    ~TouchThread()
    {}

    std::vector<GXTouch> getTouchesList()
    {
        std::vector<GXTouch> r =  _touchesList;
        _touchesList.clear();
        return r;
    }
    
    inline const GXSize &getSize() const
    {
        return _touchEvent.getSize();
    }

protected:
    
    void run();
    
private:


    TouchEvent _touchEvent;
    
    std::vector<GXTouch> _touchesList;
    
};

class GXTouchController : public Element, public Dispatch::Task
{
    
public:
    enum { Default_Time_Task = 10 };
    static GXTouchController* getInstance()
    {
        if( _ins == nullptr )
        {
            _ins = new GXTouchController();
            
        }
        return _ins;
    }
    static void retain()
    {
        _refCount++;
    }
    
    static void release()
    {
        _refCount--;
        
        if( _refCount <=0)
        {
            delete _ins;
            _ins = nullptr;
            _refCount = 0;
        }
    }


    ~GXTouchController();

    
    void setDelegate( GXTouchDelegate *delegate)
    {
        _delegate = delegate;
    }
    
    inline const GXSize &getSize() const
    {
        return _touchThread.getSize();
    }
    
private:

    GXTouchController();
    
    void doTask();
    
    GXTouchDelegate *_delegate;
    
    TouchThread _touchThread;
    
    std::list<GXTouch> _lastTouches;
    
    static GXTouchController* _ins;
    static int _refCount;
};

#endif /* GXTouch_hpp */
