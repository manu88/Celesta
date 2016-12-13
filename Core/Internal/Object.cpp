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


#include <assert.h>
#include <algorithm>

#include "Object.h"
#include "../Config.h"


void ObjectWatcher::removeObject( Object* obj )
{
    
    auto got = std::find( _objList.begin() , _objList.end(), obj );
    
    if ( got == _objList.end() )
        return;
    
    _objList.erase( got );
    
    
}

void ObjectWatcher::removeAndDeleteAllObjects(  )
{
    for( auto obj : _objList )
    {
        if (obj)
        {
            printf("delete obj %i of class %s \n" , obj->num_obj, obj->className.c_str() );
            delete obj;
        }
    }
    
    _objList.clear();
}


ObjectWatcher::~ObjectWatcher()
{
    
    /*
    removeAndDeleteAllObjects();
    
    DEBUG_ASSERT( _objList.size() == 0 );
     */
}

void ObjectWatcher::addObject( Object* obj )
{
//    printf("\t Add object %s\n" , obj->getClassName().c_str() );
    _objList.push_back(obj);
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

int Object::s_count = 0;
//ObjectWatcher Object::s_watcher;


Object::Object() noexcept 
{
    num_obj = s_count++;
    
    watcher().addObject( this );

}

Object::~Object()
{
    s_count--;
    watcher().removeObject( this );

}
