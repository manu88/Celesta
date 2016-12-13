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

#include "RunLoop.hpp"
#include "Env_Variables.h"

RunLoop::RunLoop()
{
    className = ClassNames::RunLoop;
    setGBObject( GBRunLoopInit());
}

RunLoop::~RunLoop()
{
    
}

bool RunLoop::run()
{
    return GBRunLoopRun( getGBRef() );
}

bool RunLoop::stop()
{
    return GBRunLoopStop( getGBRef() );
}

bool RunLoop::calledFromThisRunLoop() const
{
    return CalledOnRunLoop( getGBObject<const GBRunLoop*>());
}

bool RunLoop::addSource( const RunLoopSource &source)
{
    DEBUG_ASSERT(getGBRef());
    
    return false;
}

bool RunLoop::addTimer( Timer &timer)
{
    DEBUG_ASSERT(getGBRef());
    
    const bool ret = GBRunLoopAddSource( getGBObject<GBRunLoop*>(), timer.getGBRef()) ;
    
    if( ret)
    {
        timer._currentRunLoop = this;
    }
    
    return ret;
}

bool RunLoop::dispatchAsync(  GBRunLoopAsyncCallback callback, void* data)
{
    return GBRunLoopDispatchAsync(getGBObject<GBRunLoop*>(), callback, data);
}

