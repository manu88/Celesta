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
#include <algorithm>
#include "../Config.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "Thread.h"

#include "../Env_Variables.h"

int Thread::s_threadCount = 0;

Thread::Thread( const std::string threadName ) :
_isRunning  ( false ),
_shouldStop ( false ),
_thread     ( nullptr )
{
    className = ClassNames::Thread;
    
    _threadName = threadName;
    
    s_threadCount++;
}

Thread::~Thread()
{
    
    DEBUG_ASSERT( _thread == nullptr );
    
    
    s_threadCount--;
    
    
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** */

bool Thread::startThread()
{
    
    if (!_isRunning)
    {
        
        _thread = new std::thread(  std::thread ( [](Thread *self)
                                                 {
                                                     // flag as ok
                                                     self->_isRunning = true;
                                                     
                                                     self->_shouldStop = false;
                                                     
                                                     self->setThreadID();
                                                     self->run();
                                                     
                                                     // after thread's task
                                                     self->threadEnded();
                                                     
                                                 } , this));
        
        return true;
        
    }
    
    return false;
}

void Thread::setThreadID() noexcept
{
    _threadID = std::this_thread::get_id();
}

bool Thread::stopThread()
{
    _shouldStop = true;
    
    if ( _thread )
    {
        
        _thread->join();
        
        delete _thread;
        
        _thread = nullptr;
        
        return true;
    }
    
    return false;
    
}



/* **** **** **** **** **** **** **** **** **** **** **** **** */

void Thread::waitForCreation() const noexcept
{
    while ( _isRunning == false)
    {}
}

bool Thread::isRunning() const noexcept
{
    return  _isRunning;
}



void Thread::threadEnded() noexcept
{
    _isRunning = false;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** */

bool Thread::calledFromThisThread() const noexcept
{
    return std::this_thread::get_id() == _threadID;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** */
