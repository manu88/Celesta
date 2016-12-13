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

#include "Dictionnary.hpp"


Dictionary::Dictionary()
{
    setGBObject(GBDictionaryInit());
    className = ClassNames::Dictionary;
    
}


Dictionary::~Dictionary()
{
}

bool Dictionary::contains( const std::string &key) const noexcept
{
    GBString* k = GBStringInitWithCStr(key.c_str());
    
    const bool ret = GBDictionaryContains( getGBObject<const GBDictionary*>(), k);
    GBRelease(k);
    
    return ret;
}
bool Dictionary::addValueForKey(const Variant &value , const std::string &key) noexcept
{
    DEBUG_ASSERT( getGBRef() );
    
    GBString*  k = GBStringInitWithCStr(key.c_str());
    
    const bool ret = GBDictionaryAddValue(getGBObject<GBDictionary*>(), value.getGBRef(), k);
    GBRelease(k);
    return ret;
    
}

Variant Dictionary::getValueForKey( const std::string &key) const noexcept
{
    GBString*  k = GBStringInitWithCStr(key.c_str());
    GBRef v= GBDictionaryGetValueForKey( getGBObject<const GBDictionary*>(), k);
    GBRelease(k);
    
    if( v)
        return Variant(v);
    
    return Variant::null();
    
}