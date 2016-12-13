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
#include <time.h>

#include "AbstractController.h"
#include "../Scheduler/Chrono.h"
#include "../Log/Log.h"

std::unordered_set< AbstractController *> AbstractController::s_controllers;

AbstractController::AbstractController( const std::string &name ) :
_isReady    ( false ),
_isInactive ( false ),
_controllerName( name )
{
    s_controllers.insert( this );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

AbstractController::~AbstractController()
{
    s_controllers.erase( this);
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ bool AbstractController::allControllersReady() noexcept
{
    return countReadyControllers() == static_cast< int >( s_controllers.size() );
}

/*static*/ int AbstractController::countReadyControllers() noexcept
{
    int count = 0;
    for (auto i : s_controllers )
    {
        if( i->isReady() || i->isInactive() )
            count++;
    }
    
    return count;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ bool AbstractController::allControllersUnReady() noexcept
{
    for (auto i : s_controllers )
    {
        if(  i->isReady() && i->isActive() )
            return false;
    }
    
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ bool AbstractController::waitForAllControllersToBeReady(int timeOut /*SECS*/) noexcept
{
    clock_t start = clock();
    double time = 0;
    
    while ( (time <= timeOut)  )
    {
        if ( allControllersReady() )
        {
            return true;
        }
        
        time = ( clock() - start)/(double)CLOCKS_PER_SEC;
    }
    

    

    return false;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ bool AbstractController::waitForControllerToBeReady( AbstractController*controller , int timeOut )
{
    clock_t start = clock();
    double time = 0;

    while ( (time <= timeOut)  )
    {
        if ( controller->isReady() || controller->isInactive() )
        {
            return true;
        }

        time = ( clock() - start)/(double)CLOCKS_PER_SEC;
    }
    
    return false;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ bool AbstractController::waitForControllerToFinish( AbstractController*controller , int timeOut /*= 10 SECS*/)
{
    clock_t start = clock();
    double time = 0;
    
    
    
    while ( (time <= timeOut)  )
    {
        if ( controller->isReady() || controller->isInactive() )
        {
            return true;
        }
        
        
        time = ( clock() - start)/(double)CLOCKS_PER_SEC;
    }
    
    return false;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ bool AbstractController::waitForAllControllersToFinish(int timeOut /*SECS*/)
{
    clock_t start = clock();
    double time = 0;

    while ( (time <= timeOut )  )
    {
        if ( allControllersUnReady() )
            return true;
        
        time = ( clock() - start)/(double)CLOCKS_PER_SEC;
    }
    
    return false;

}
