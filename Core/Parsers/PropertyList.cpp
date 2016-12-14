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

#include "PropertyList.hpp"
#include "../Env_Variables.h"

PList::Document::Document()
{
    setGBObject(GBPListInit());
    className = ClassNames::PListDocument;
    
}

PList::Document::Document( const std::string &file)
{
    className = ClassNames::PListDocument;
    
    GBString* fileP = GBStringInitWithCStr( file.c_str());
    
    setGBObject( GBPListInitWithFile(fileP) );
    
    GBRelease(fileP);
}

PList::Document::~Document()
{

}



bool PList::Document::addValueForKey( const Variant &value , const std::string &key) noexcept
{
    DEBUG_ASSERT( getGBRef() );
    
    GBString*  k = GBStringInitWithCStr(key.c_str());
    
    const bool ret = GBPListAddValue( getGBObject<GBPList*>() , value.getGBRef(), k);
    GBRelease(k);
    return ret;
}

Variant PList::Document::getValueForKey( const std::string &key) const noexcept
{
    GBString*  k = GBStringInitWithCStr(key.c_str());
    GBRef v= GBPListGetValueForKey( getGBObject< const GBPList* >() , k);
    GBRelease(k);
    
    if( v)
        return Variant(v);
    
    return Variant::null();
}


bool PList::Document::save(const std::string &file)
{
    GBString*  f = GBStringInitWithCStr( file.c_str() );
    const bool ret = GBPListSave(getGBObject<GBPList*>() , f);
    GBRelease(f);
    
    return ret;
}


bool PList::Document::contains( const std::string &key) const noexcept
{
    GBString* k = GBStringInitWithCStr(key.c_str());
    
    const bool ret = GBPListContains(getGBObject<const GBPList*>() , k);
    GBRelease(k);
    
    return ret;
}

bool PList::Document::remove(const std::string &key) noexcept
{
    GBString* k = GBStringInitWithCStr(key.c_str());
    
    const bool ret = GBPListRemove(getGBObject< GBPList*>() , k);    GBRelease(k);
    
    return ret;
}
