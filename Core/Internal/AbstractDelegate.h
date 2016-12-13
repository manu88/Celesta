/*
 * Celesta
 *
 * Copyright (c) 2015 Manuel Deneu
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

#ifndef __MediaCenter__AbstractDelegate__
#define __MediaCenter__AbstractDelegate__

class AbstractController;

class AbstractDelegate
{
public:

    virtual ~AbstractDelegate();


protected:
    AbstractDelegate() noexcept;
    
    
    //! Controllers will call this method to check if everything is ok.
    //! When false is returned, the callback is ignored, and this incomming event is lost.
    //! when not reimplemented by sub-classes, the defaulf impl. will always return true;
    virtual bool delegateReadyForController( const AbstractController* controller );
    


    
    
};

#endif /* defined(__MediaCenter__AbstractDelegate__) */
