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

#ifndef GBObjectWrapper_h
#define GBObjectWrapper_h

#include "../../../FlyLab/GroundBase/include/GroundBase.h"

class GBObjectWrapper 
{
public:
    virtual ~GBObjectWrapper();
    
    
    GBObjectWrapper& operator=(GBObjectWrapper const& copy);
protected:
    GBObjectWrapper();
    
    void setGBObject(void* newObject);

    GBObject* getGBRef()
    {
        return _gbObj;
    }
    
    GBObject* getGBRef() const
    {
        return _gbObj;
    }
    
    template<typename Type>
    Type getGBObject() const
    {
        return static_cast<Type>( _gbObj );
    }
    
private:
    void *_gbObj;
    
    
};

#endif /* GBObjectWrapper_h */
