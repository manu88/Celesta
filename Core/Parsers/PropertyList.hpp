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

#ifndef PropertyList_hpp
#define PropertyList_hpp

#include "../Internal/Element.h"
#include "../Internal/GBObjectWrapper.hpp"
#include "../Data/SequenceType.hpp"

/*
 This class is a GBPListDocument inside!
 Plist::Document is GroundBase's class counterpart of GBPlistDocument 
 */
namespace PList
{
    class Document : public SequenceType, public Element , public GBObjectWrapper
    {
    public:
        Document();
        Document( const std::string &file);
        ~Document();
        
        
        bool addValueForKey( const Variant &value , const std::string &key) noexcept;
        Variant getValueForKey( const std::string &key) const noexcept;
        bool remove(const std::string &key) noexcept;
        
        bool contains( const std::string &key) const noexcept;
        
        GBSize getSize() const;
        
        bool save(const std::string &file);
        
    };
}

#endif /* PropertyList_hpp */
