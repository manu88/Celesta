//
//  Object.cpp
//  Broadway_test
//
//  Created by Manuel Deneu on 17/09/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//


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
