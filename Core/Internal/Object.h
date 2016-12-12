//
//  Object.h
//  Broadway_test
//
//  Created by Manuel Deneu on 17/09/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_test__Object__
#define __Broadway_test__Object__

#include <iostream>
#include <list>
#include <set>
#include <mutex>

#include "../Data/Variant.h"




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
