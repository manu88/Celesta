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
#ifndef Broadway_test_Config_h
#define Broadway_test_Config_h

extern "C"
{
    #include "../../FlyLab/GroundBase/include/GroundBase.h"
}




struct ScopedGBRef
{
    ScopedGBRef( GBRef val):
    value(val)
    {}
    
    ~ScopedGBRef()
    {
        GBRelease( value);
    }
    
    GBRef value;
    
};


#endif
