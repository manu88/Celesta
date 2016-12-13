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

#include "Timer.hpp"

#include "Env_Variables.h"


void TimerDelegate::onTime()
{
    printf("On time\n");
}

/* **** **** **** **** **** **** **** **** **** **** **** **** */

Timer::Timer():
_currentRunLoop(nullptr),
_delegate(nullptr)
{
    className = ClassNames::Timer;
    setGBObject( GBTimerInit( timerCallback ));
 
    GBRunLoopSourceSetUserContext( getGBObject<GBTimer*>(), this);
}

Timer::~Timer()
{
    
}

bool Timer::setInterval(GBTimeMS interval)
{
    return GBTimerSetIntervalMS( getGBObject<GBTimer*>(),interval);
}


GBTimeMS Timer::getInterval() const
{
    return GBTimerGetIntervalMS( getGBObject<const GBTimer*>() );
}

bool Timer::setPeriodic(bool periodic)
{
    return GBTimerSetPeriodic( getGBObject<GBTimer*>(), periodic? 1 : 0);
}

bool Timer::isPeriodic() const
{
    return GBTimerIsPeriodic( getGBObject<const GBTimer*>() );
}

bool Timer::isActive() const
{
    return GBTimerIsActive(getGBObject<const GBTimer*>() );
}
bool Timer::setActive( bool active)
{
    return GBTimerSetActive(getGBObject<GBTimer*>(), active? 1 : 0);
}

RunLoop* Timer::getRunLoop() const noexcept
{
    return _currentRunLoop;
}

/*static*/ void Timer::timerCallback( GBRunLoopSource* source , GBRunLoopSourceNotification notification)
{
    DEBUG_ASSERT(source);
    
    Timer* self = static_cast<Timer*>( GBRunLoopSourceGetUserContext(source) );
    
    if( self->_delegate)
    {
        self->_delegate->_timer = self;
        self->_delegate->onTime();
    }
    
}
