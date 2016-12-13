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

#ifndef __JSMachine__JSMachineDelegate__
#define __JSMachine__JSMachineDelegate__


#include "../Internal/Object.h"


//! JSMachineDelegate
/********************************************//**
*
*   An instance of this Class catches notification from a JSMachine object.
*
***********************************************/

class Selector; // forward

class JSMachineDelegate 
{
public:
    virtual ~JSMachineDelegate();
    
    //! This method needs to be subclassed by your class.
    /*!
     \param selector The called selector
     \sa JSMachine, Selector
     */
    virtual void functionCalled( const Selector *selector ) =0;
    
protected:
    JSMachineDelegate();

private:
    
    
    
};
#endif /* defined(__JSMachine__JSMachineDelegate__) */
