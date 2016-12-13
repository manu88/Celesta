/*
 * Celesta
 *
 * Copyright (c) 2014 Manuel Deneu
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

#ifndef __Broadway_test__Thread__
#define __Broadway_test__Thread__

#include <thread>
#include <condition_variable>

#include "Object.h"
#include "../Scheduler/Timecode.h"


typedef std::unique_lock<std::mutex>    ScopedLock;

class Thread : public virtual Object
{
public:

    virtual ~Thread();
    
    bool startThread();
    bool stopThread();
    bool isRunning() const noexcept;

    
    /* **** **** **** **** */
    /*
    inline void wakeUpThread() const noexcept
    {
        _wakeUp.notify_all();
        
    }
    
    inline void wait( ScopedLock &lock)
    {
        _wakeUp.wait( lock );
        
    }
    
    inline void waitUntil( ScopedLock &lock , std::chrono::time_point<std::chrono::steady_clock> timestamp )
    {
        _wakeUp.wait_until( lock , timestamp );
    }
    
    inline std::mutex &getMutex()
    {
        return _mutex;
    }
     */
    /* **** **** **** **** */    
    
    bool calledFromThisThread() const noexcept;
    
    static int getThreadCount()
    {
        return s_threadCount;
    }
    
    static inline void sleepFor( const Timecode &tc)
    {
        sleepForMs( tc.getInMs() );
    }
    
    static inline void sleepForMs( const unsigned long ms)
    {
        std::this_thread::sleep_for (std::chrono::milliseconds( ms ) );
    }
    static inline void sleepForMicros( const unsigned long micros )
    {
        std::this_thread::sleep_for (std::chrono::microseconds( micros ) );
    }
    
protected:
    
    Thread( const std::string threadName);
    
    // user defined main() thread
    virtual void run() = 0;
    
    bool threadShouldStop() const noexcept
    {
        return _shouldStop;
    }
    
    void sendAsyncStop() noexcept
    {
        _shouldStop = true;
    }

    void waitForCreation() const noexcept;
    
    
    void setThreadID() noexcept;
    
private:
    
    void threadEnded() noexcept;

    volatile bool       _isRunning;
    volatile bool       _shouldStop;
    
    
    std::string         _threadName;
    
    /* thread pool */
    std::thread      *_thread;
    std::thread::id   _threadID;
    
    
    static int s_threadCount;
};

#endif /* defined(__Broadway_test__Thread__) */
