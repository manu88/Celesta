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

#include <assert.h>
#include <string.h>
#include "JSONParser.h"
#include "../Env_Variables.h"
#include "../Internal/FileSystem.h"
#include "../Log/Log.h"


const JSON::Node JSON::Node_Iterator::operator*()
{
    return (Node(  cJSON_GetArrayItem( _element, _pos) , _element ));
}
/*
const JSON::Node* JSON::Node_Iterator::operator->()
{
    return (operator*());
}
*/
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

JSON::Document::Document()
{
    setGBObject( GBJSONDocumentInit() );
    
    className = ClassNames::JSON_Document;
}

JSON::Document::Document( const std::string &file)
{
    const GBString* f = GBStringInitWithCStr( file.c_str() );
    
    setGBObject( GBJSONDocumentInitWithFile(f) );
    
    GBRelease(f);
    className = ClassNames::JSON_Document;
}

JSON::Document::Document( const char* data , const GBSize dataSize)
{
    setGBObject( GBJSONDocumentInitWithBuffer(data, dataSize) );
    className = ClassNames::JSON_Document;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

JSON::Document::~Document()
{

}

bool JSON::Document::isValid() const
{
    return getGBRef() != nullptr;
}
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool JSON::Document::addValueForKey(const Variant &value , const std::string &key) noexcept
{
    const GBString* k = GBStringInitWithCStr( key.c_str());
    
    const bool ret = GBJSONDocumentAddValue( getGBObject<GBJSONDocument*>() , value.getGBRef(), k );
    GBRelease( k );
    
    return ret;
}

Variant JSON::Document::getValueForKey( const std::string &key) const noexcept
{
    const GBString*  k = GBStringInitWithCStr(key.c_str());
    GBRef v= GBJSONDocumentGetValueForKey( getGBObject< const GBJSONDocument* >() , k);
    GBRelease(k);
    
    if( v)
        return Variant(v);
        
        return Variant::null();
}

bool JSON::Document::contains( const std::string &key) const noexcept
{
    const GBString* k = GBStringInitWithCStr(key.c_str());
    
    const bool ret = GBJSONDocumentContains(getGBObject<const GBJSONDocument*>() , k);
    GBRelease(k);
    
    return ret;
}

bool JSON::Document::remove(const std::string &key) noexcept
{
    const GBString* k = GBStringInitWithCStr(key.c_str());
    
    const bool ret = GBJSONDocumentRemove(getGBObject< GBJSONDocument*>() , k);    GBRelease(k);
    
    return ret;
}


bool JSON::Document::save( const std::string &file ,bool minify)
{
    return GBJSONDocumentSave(getGBObject< GBJSONDocument*>());
}

GBSize JSON::Document::getSize() const
{
    return GBJSONDocumentGetSize(getGBObject< const GBJSONDocument*>());
}

const std::string JSON::Document::stringify(const JSON::Node &node )
{
    std::string ret;
    char* result = cJSON_Print( node._node );
    
    ret = result;
    
    free(result );
    return  ret;
    
}




/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */





