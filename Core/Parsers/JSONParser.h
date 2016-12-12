//
//  JSONDocument.h
//  Broadway_core
//
//  Created by Manuel Deneu on 08/01/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_core__JSONDocument__
#define __Broadway_core__JSONDocument__

#include <iostream>
#include <memory>

#include "../Internal/Element.h"

#include "cJSON/cJSON.h"


namespace JSON
{


typedef enum
{
    Node_False = 0,
    Node_True = 1,
    Node_NULL = 2,
    Node_Number = 3,
    Node_String = 4,
    Node_Array = 5,
    Node_Object = 6,
    
} NodeType;
    
    

class Node;
    
class Node_Iterator
{
public:
    Node_Iterator( cJSON* node , int pos):
    _element ( node ),
    _pos  ( pos  )
    {}
    
    
    const Node operator*();
    
    //const Node *operator->();
    
    
    /**/

    Node_Iterator &operator++()
    {
        ++_pos;
        return *this;
    }
    Node_Iterator operator++(int)
    {
        Node_Iterator tmp(*this);
        ++(*this);
        return tmp;
    }
    
    bool operator==( const Node_Iterator &rhs)
    {
        return _pos == rhs._pos;
    }
    
    bool operator!=( const Node_Iterator &rhs)
    {
        return _pos != rhs._pos;
    }

    /**/
    
private:
    cJSON*  _element;
    cJSON *_current;
    int _pos;
};

    
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
    
class Node
{
    friend class Document;
    friend class Node_Iterator;
    
public:
    
    static Node invalid()
    {
        return Node();
    }
    
    Node( cJSON *node = nullptr ,cJSON *parent = nullptr ):
    _owned  ( false  ),
    _node   ( node   ),
    _parent ( parent )
    {
        if( _node == nullptr)
        {
            _node = cJSON_CreateObject();
        }

        _type = ( NodeType ) _node->type;
        
        if( _parent )
            _owned = true;

    }
    
    ~Node()
    {
        if( !_owned )
            cJSON_Delete( _node );
        
        _node = nullptr;
    }
    
    typedef Node_Iterator iterator;
    
    iterator begin() const
    {
        return iterator( _node , 0);
    }
    
    iterator end() const
    {
        return iterator( _node , size() );
    }
    
    Node createNode(  const std::string &text)
    {
        return Node( cJSON_CreateString(text.c_str()), _node);
    }
    
    Node createNode(  const Variant &v)
    {
        return  Node( createInternalNode(v) , _node);
    }
    
    cJSON* createInternalNode(  const Variant &v)
    {
        if( v.isBool())
            return cJSON_CreateBool( v.getInt() );
        
        else if (v.isNumeric())
            return cJSON_CreateNumber( v.getDouble() );
        
        else if (v.isString())
            return cJSON_CreateString( v.getString().c_str() );
        
        else if( v.isDataPair())
            return createInternalNode( v.getPair().second );

        else if (v.isList())
        {
            cJSON *lst = cJSON_CreateArray();
            
            for ( const auto &i : v.getList())
                cJSON_AddItemToArray(lst,createInternalNode(i) ) ;
            
            return lst;
        }
        
        
        return nullptr;
    }
    
    operator bool() const
    {
        return _node != nullptr;
    }
    
    const std::string getName() const
    {
        if ( _node && _node->string)
            return _node->string;
 
        return std::string();
    }
    
    const int getAttribsSize() const
    {
        return getArraySize();
    }
    
    /* ---- typecheck ----  */
    
    bool isValid() const
    {
        if( _node == nullptr)
            return false;
        
        return true;
    }
    
    inline bool isInvalid() const
    {
        return !isValid();
    }
    
    /* ** */
    
    NodeType getType() const
    {
        return _type;
    }
    
    bool isObject() const
    {
        return _type == Node_Object;
    }
    
    bool isString() const
    {
        return _type == Node_String;
    }
    
    bool isNumber() const
    {
        return _type == Node_Number;
    }
    bool isBool() const
    {
        return _type == Node_False || Node_True;
    }
    
    bool isNull() const
    {
        return _type == Node_NULL;
    }
    
    bool isArray() const
    {
        return _type == Node_Array;
    }
    
    /* ----  get vals ---- */
    
    Variant getValue() const
    {
        if (isString())
            return Variant(getString());
        
        else if ( isNumber())
            return Variant( getDouble() );
        
        else if (isArray() )
        {
            VariantList list;
            for (int i=0; i<getArraySize(); i++)
            {
                const Node &n = getArrayItem(i);
                
                list.push_back( n.getValue() );

            }
            return Variant( list );
        }
        
        return Variant::null();
    }
    
    const std::string getString() const
    {
        if( _node->valuestring )
            return _node->valuestring;
        
        return "";
    }
    
    
    /*
     numeric values:
     
     Warning : there's no way to tell if a numeric value type is integer or double.
     If getInt() is called on a float, the returned value will be truncated.
     In doubt, better call Saul, heuu no,  better call getDouble()
     */
    
    double getDouble() const
    {
        return _node->valuedouble;
    }
    
    int getInt() const
    {
        return _node->valueint;
    }
    
    bool getBool() const
    {
        return _node->type == Node_False? false : true;
    }
    
    /* ----  set vals ---- */
    
    Node addObject( const std::string &name)
    {
        cJSON *obj = nullptr;
        if( isObject())
        {
            obj = cJSON_CreateObject();
            cJSON_AddItemToObject(_node, name.c_str(), obj );
        }
        
        else if (isArray())
        {
            obj = cJSON_CreateObject();
            cJSON_AddItemToArray(_node, cJSON_CreateObject() );

        }
        return Node(obj , _node );
    }
    
    Node addArray( const std::string &name)
    {
        cJSON *array = nullptr;
        if( isObject())
        {
            array  = cJSON_CreateArray();
            cJSON_AddItemToObject(_node, name.c_str(), array);
        }
        else if (isArray())
        {
            array  = cJSON_CreateArray();
            cJSON_AddItemToArray(_node,  array);

        }
        
        return Node(array , _node);
    }
    
    bool addItemToArray( Node &node)
    {
        node._owned = true;
        if ( isArray() )
        {
            cJSON_AddItemToArray(_node,node._node);
            return true;
        }
        
        return false;
    }
    
    bool addItemToArray( const Variant &v)
    {
        if ( isArray() )
        {
            cJSON_AddItemToArray(_node,createInternalNode( v));
            return true;
        }
        
        return false;
    }
    
    bool addItemToObject(const Variant &v , const std::string &name  )
    {
        if ( isObject() )
        {
            if (v.isDataPair())
                cJSON_AddItemToObject(_node, v.getPair().first.c_str() , createInternalNode(v) );
            else
                cJSON_AddItemToObject(_node, name.empty()?nullptr: name.c_str() , createInternalNode(v) );
            return true;
        }
        return false;
    }
    
    void addItem( const std::string &name ,const Node& node)
    {
        cJSON_AddItemToObject(_node, name.c_str() , node._node);
    }
    
    void addItem( const std::string &name ,const Variant &v)
    {
        cJSON_AddItemToObject(_node, name.c_str() , createNode( v )._node );
    }
    
    
    
    bool removeItem( const std::string &name)
    {
        cJSON *f =cJSON_GetObjectItem(_node, name.c_str() ) ;
        
        if (f)
        {
            cJSON_DetachItemFromObject(_node, name.c_str());
            cJSON_Delete( f );
            return true;
        }
        
        return false;
    }
    
    int size() const
    {
        if (isArray() || isObject() )
            return getArraySize();
        
        return -1;
        
    }
    
    int getArraySize() const
    {
        return cJSON_GetArraySize( _node );
    }
    
    
    Node getArrayItem( int index ) const
    {
        return Node( cJSON_GetArrayItem(_node, index ) , _node );
    }
    
    // child
    
    bool hasChild() const
    {
        return _node->child != nullptr;
    }
    
    Node getChild() const
    {
        return Node( _node->child  , _node );
    }
    
    // next sibling
    
    bool hasNext() const
    {
        return _node->next != nullptr;
    }
    
    Node getNext() const
    {
        return Node( _node->next  ,_node );
    }
    
    Node getObjectItem( const std::string &attribName ) const
    {
        return Node( cJSON_GetObjectItem( _node, attribName.c_str() ) , _node );
    }
    

    /* ** */
    
    void clear()
    {
        cJSON_Delete( _node );
        _node = nullptr;
    }
    
private:
    

    
    bool     _owned;
    cJSON   *_node;
    cJSON   *_parent;
    NodeType _type;
    
    
};
    


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
    
class Document : public Element
{
public:
    
    Document();
    virtual ~Document();
    
    bool parseFile( const std::string &file );
    bool parseBuffer( const char* txtBuff);

    static const std::string stringify( const JSON::Node &node );
    bool saveFile( const std::string &file , bool minify);
    
    bool hasErrors() const noexcept
    {
        return _hasErrors;
    }
    
    const std::string &getError() const noexcept
    {
        return _firstError;
    }

    
    // return the root of the JSON database. can be NULL!!
    Node &getRootNode()
    {
        /*
        if( _json == nullptr)
            createRootNode();
        */
        return _root;//  Node( _json , nullptr) ;
    }
    
    // will return nullptr if not found
    Node getNode( const std::string &name) const;
protected:
    

private:
    
    bool createRootNode();
    
    bool _hasErrors;
    std::string _firstError;
    Node _root;

};

} // namespace JSON
#endif /* defined(__Broadway_core__JSONDocument__) */
