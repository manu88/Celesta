//
//  JSONDocument.cpp
//  Broadway_core
//
//  Created by Manuel Deneu on 08/01/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

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

JSON::Document::Document() :
_hasErrors( false ),
_root( Node(nullptr , nullptr ) )
{
    className = ClassNames::JSON_Document;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

JSON::Document::~Document()
{
    /*
    if ( _json != nullptr )
        cJSON_Delete( _json );
    */
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool JSON::Document::parseFile( const std::string &file )
{

    if ( !FileSystem::fileExists( file ) )
        return false;
    
    bool ret = false;
    
    FILE *f=fopen(file.c_str() ,"rb");
    
    fseek(f,0,SEEK_END);
    
    const long len = ftell( f );
    
    fseek(f,0,SEEK_SET);
    
    char *data=(char*)malloc( static_cast<size_t>( len+1 ) );
    
    fread( data , 1 , static_cast<size_t>( len ) , f );
    
    fclose( f );
    
    ret = parseBuffer( data );

    free( data );
    
    return ret;

}

bool JSON::Document::parseBuffer( const char* txtBuff)
{
    _root.clear();

    _firstError.clear();
    
    bool ret = false;
    
    _root._node =cJSON_Parse( txtBuff );
    
    
    _hasErrors = cJSON_GetErrorPtr() != 0;
    
    if ( _hasErrors )
    {
        ret = false;
        const char* err = cJSON_GetErrorPtr();
        
        const size_t totalLength = strlen( err );
        
        const size_t ss = totalLength >= 20 ? 20 : totalLength;
        
        char b[ ss ];
        strncpy( b, err, ss );
        _firstError = b;

    }
    
    else /* We're good here */
        ret = true;


    return ret;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
const std::string JSON::Document::stringify(const JSON::Node &node )
{
    std::string ret;
    char* result = cJSON_Print( node._node );
    
    ret = result;
    
    free(result );
    return  ret;
    
}
bool JSON::Document::saveFile( const std::string &file ,bool minify)
{
    
    char* result = cJSON_Print( _root._node );
    
    if (minify)
        cJSON_Minify( result);
    
    bool res = false;
    
    if( result )
    {

        if (FileSystem::setFileText( file , result ))
            res = true;
            
        free( result );

    }
    return res;
}



/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

JSON::Node JSON::Document::getNode( const std::string &name) const
{
    cJSON* currentNode = _root._node;
    cJSON* parent      = nullptr;
    while ( currentNode != nullptr)
    {
        if ( currentNode->string && ( strcmp(name.c_str(), currentNode->string) == 0 ) )
            return Node( currentNode , parent );
        
        if ( currentNode->child)
        {
            parent = currentNode;
            currentNode = currentNode->child;

        }
        
        else if ( currentNode->next )
        {
            parent = currentNode;
            currentNode = currentNode->next ;
        }
        
        else
            currentNode = nullptr;
    }
    
    
    return Node( nullptr );
}

bool JSON::Document::createRootNode()
{
    /*
    if( _json != nullptr)
        return false;
    
    _json = cJSON_CreateObject();
    */
//    _root = Node(_json , nullptr);
    
    return true;
}



