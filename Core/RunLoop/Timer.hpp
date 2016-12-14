/*
 * Celesta
 *
 * Copyright (c) 2016 Manuel Deneu
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef Timer_hpp
#define Timer_hpp

#include "../Internal/Element.h"
#include "../Internal/GBObjectWrapper.hpp"
#include "RunLoopSource.hpp"

class Timer;
class RunLoop;
class TimerDelegate
{
    friend class Timer;
public:
    virtual void onTime();
    
    
protected:
    TimerDelegate():_timer(nullptr)
    {}
    
    Timer* getTimer() const
    {
        return _timer;
    }
    
private:
    Timer* _timer;
};

class Timer : public Element,
              public RunLoopSource,
              public GBObjectWrapper
{
    friend class RunLoop;
    
public:
    
    Timer();
    ~Timer();
    
    bool setInterval(GBTimeMS interval);
    GBTimeMS getInterval() const;
    
    bool setPeriodic(bool periodic);
    bool isPeriodic() const;
    
    bool isActive() const;
    bool setActive( bool active);
    
    void setDelegate(TimerDelegate* delegate)
    {
        _delegate = delegate;
    }
    
    TimerDelegate* getDelegate() const
    {
        return _delegate;
    }
    
    RunLoop* getRunLoop() const noexcept;
    
protected:
private:

    RunLoop* _currentRunLoop;
    TimerDelegate *_delegate;
    static void timerCallback( GBRunLoopSource* source , GBRunLoopSourceNotification notification);
};

#endif /* Timer_hpp */
