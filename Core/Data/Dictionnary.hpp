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

#ifndef Dictionnary_hpp
#define Dictionnary_hpp

#include "../Config.h"
#include "../Internal/GBObjectWrapper.hpp"
#include "Env_Variables.h"
#include "SequenceType.hpp"

class Dictionary : public SequenceType, public Object, public GBObjectWrapper
{
public:
    
    Dictionary();
    ~Dictionary();
    
    
    
    bool contains( const std::string &key) const noexcept;
    bool addValueForKey(const Variant &value , const std::string &key) noexcept;
    Variant getValueForKey( const std::string &key) const noexcept;
    bool remove(const std::string &key) noexcept;
    
    GBSize getSize() const noexcept;
};

#endif /* Dictionnary_hpp */
