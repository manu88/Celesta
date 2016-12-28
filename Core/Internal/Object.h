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

#ifndef __Broadway_test__Object__
#define __Broadway_test__Object__

#include <iostream>
#include <list>
#include <set>
#include <mutex>

//#include "../Data/Variant.h"



/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
class Object;

class ObjectWatcher
{
public:
    ObjectWatcher()
    {}
    
    ~ObjectWatcher();
    
    void addObject( Object* obj );
    
    void removeObject( Object* obj );
    void removeAndDeleteAllObjects( );

private:
    
    std::list<Object*> _objList;

};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */



class Object 
{
    friend class ObjectWatcher;
public:
    
    int getId() const noexcept
    {
        return num_obj;
    }
    
    const std::string &getClassName() const noexcept
    {
        return className;
    }
    
    /* **** **** **** **** **** **** **** **** **** */

protected:
    Object() noexcept;
    
    // copy ctor is disabled
    Object( const Object&) = delete ;

    virtual ~Object();
    
    //! A good way to keep track of objects.
    /* \
       \ className will be printed when an error occur in Object's management,
       \ e.g when ObjectWatcher has to remove any instance.
     */
    std::string className;
    int num_obj;
    //GBObject* _gbPtr; // pointer to GBObject instance. Can be NULL if the object is not an GRoundBase object
    
private:
    
    
    
    //const Object *_parentObject;
    
    static int s_count;
    
    
    static ObjectWatcher &watcher()
    {
        static ObjectWatcher w = ObjectWatcher();
        return w;
    }
    
};

#endif /* defined(__Broadway_test__Object__) */
