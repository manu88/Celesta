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

#ifndef __JSMachine__JSMachine__
#define __JSMachine__JSMachine__

#include <iostream>
#include <string.h>
#include <vector>



#include "../Internal/Object.h"
#include "../Internal/Element.h"

#include "../Data/Database.h"


#define TINYJS_CALL_STACK
#include "TinyJS/TinyJS.h"

#ifdef USE_JSON_PARSER
#include "../Parsers/JSONParser.h"
#endif


class JSMachineDelegate; // forward


//! Selector
/********************************************//**
*
*    Class for callback from JSMachine's to JSMachineDelegate's instances
*
***********************************************/
class Selector : public Object
{
public :
    

    //! Selector's Constructor
    /*!
     \param _signature the JavaScript signature of the called function without ';'. E.g: foo( var1 , var2 ).
     \param _delegate  the instance JSMachineDelegate on whitch to call the function.
     \sa JSMachine, JSMachineDelegate
     */
    Selector(const std::string &_signature , JSMachineDelegate* _delegate);
    
    
    ~Selector();
    
    void callFunctionwithVars(CScriptVar *vars);
    

    
    //! The signature of the function passed in the constructor.
    std::string         signature;
    
    //! The name of the function
    /*!
        Eg : signature = "foo(var1)" -> identifier = "foo". 
        This variable can be used for comparisons, or retrieve a selector.
     */
    std::string         identifier;
    
    //! A ref to the JSMachineDelegate's instance
    /*!
        This pointer is ref-only, it will not be deleted!
     */
    JSMachineDelegate  *delegate;

    //! The set of variables to pass with the call
    /*!
        See TinyJS's Documentation
     */
    CScriptVar         *variables; // refs

    //! Equality comparison between to instances of Selector
    bool operator==( const Selector& rhs);
};


namespace JSDataType
{

    static const std::string Int        = "Integer";
    
    static const std::string Double     = "Double";
    static const std::string String     = "String";
    static const std::string Function   = "Function";
    static const std::string Object     = "Object";
    static const std::string Array      = "Array";
//    Native     = 6,
    static const std::string Undefined  = DefaultsValues::Undefined;
    static const std::string Null       = "Null";

    static const std::string Error       = "Error";
}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** ****  */

//! JSMachine
/********************************************//**
*
*    This class can parse js files, run a real time interpreter, and notify a delegate when a function is called using Selectors.
*
*    In order to parse & interpret javascript code, TinyJS is used.
*    See https://code.google.com/p/tiny-js/                                               
***********************************************/

class JSMachine : public Object
{
public:
    JSMachine();

    ~JSMachine();
    
    void allowSystemCalls( bool shouldI )
    {
        _allowSystemCall = shouldI;
    }
    

    //! Sets the delegate that will be called
    /*!
     \param delegate The subclass of JSMachineDelegate you want to be notified
     \sa Selector, JSMachineDelegate
     */
    void setDelegate( JSMachineDelegate *delegate )
    {
        _delegate = delegate;
    }

    //! Add and register a JS function to the look-up table
    /*!
     \param signature The function's signature. E.g: "foo(var1 , var2)". 
     */
    void registerFunctionWithSignature(const std::string &signature);


    
    //! Remove a Selector from the look-up table
    /*!
     \param signature The signature of the function to remove. 
     */
    void removeRegisteredFunctionWithSignature(const std::string &signature);
    
    //! Get the registered functions' count.
    /*!
     \return The look-up table' size.
     */
    int getRegisteredFunctionsCount() const
    {
        return ( int ) _registeredSelectors.size();
    }
    
    //! Objective-C style test method.
    /*!
     \param signature The signature to test.
     \return True if the signature can be called.
     */
    bool respondsToSelector(const std::string &signature);
    
    
    //! Text file Parser
    /*!
     \param filename The JS file to parse.
     \return True if no error occured during parsing.
     */
    bool parseScriptFile( const std::string &filename );
    
    bool importScriptFile( const std::string &filename );
    
    //! Evaluate the content of a text buffer
    /*!
     \param buffer The buffer to parse.
     \return true if no errors.
     */
    bool executeBuffer(  const std::string &buffer);
    
    // without ";" !!!!
    CScriptVarLink evaluate( const std::string &buffer);
    std::string    evaluateAsString(const std::string &buffer);

    void reset();
    
    
    static VariantList getArgumentsFromJSArray( CScriptVar *vars);
    
    static CScriptVar* getArgumentsAsJSArray( const VariantList &list );
    static CScriptVar* getArguments( const Variant *var);
    
#ifdef USE_JSON_PARSER
    static CScriptVar* JSonParse( const JSON::Node &node);
    
    // see jsonParseRecursive bellow
#endif
    
    static std::string getArgumentsAsJSArrayString ( const VariantList &list );
    static std::string getArgumentsAsStringList    ( const VariantList &list );
    
#ifdef ENABLE_ELEMENT_SELECTOR
    static std::string getCallFromSelector( const std::string &selector , const VariantList  &arguments );
#endif
    
    template<typename Type>
    static CScriptVar* getJSArrayFromVector( const std::vector<Type> &vect)
    {
        if ( vect.empty() )
            return nullptr;
        
        CScriptVar* ret = new CScriptVar();
        
        ret->setArray();
        
        int count = 0;
        
        for ( auto val : vect)
        {
            ret->setArrayIndex(count, new CScriptVar( val ));
            count++;
        }
        
        return ret;
    }
    
    template<typename Type>
    static CScriptVar* getJSArrayFromDatabase( const Database &data)
    {
        const size_t size = data.getSize();
        if ( size == 0 )
            return nullptr;
        
        CScriptVar* ret = new CScriptVar();
        
        ret->setArray();
        
        for ( size_t i = 0; i < size; i++)
        {
            const std::string name = data.getItemNameAtIndex( i );
            const Type value = data.getValueAtIndex( i );
            
            CScriptVar *item = new CScriptVar();
            item->setArray();
            
            item->setArrayIndex(0, new CScriptVar( name ));
            item->setArrayIndex(1, new CScriptVar( value ));
            
            ret->setArrayIndex( static_cast<int>( i ), item);
        }
        
        return ret;
    }

    static std::string getDatabaseAsJSArrayString( const Database &data)
    {
        std::ostringstream stream;
        
        const size_t size = data.getSize();

        if ( size == 0 )
            return DefaultsValues::Undefined;
        
        stream << "[";


            
        for ( size_t i = 0; i< size; i++)
        {
            if (i != 0)
                stream << ",";
            
            stream << "["
            << "\"" << data.getItemNameAtIndex( i ) << "\""
            << ","
            << "\"" << data.getValueAtIndex( i ).getString() << "\""
            << "]";
            
        }

        stream << "]";
        
        return stream.str();
    }
    
    
protected:
    
    void prepareEnvironment();

    // called by reset() only!
    void addRegisteredSelectors();
    
    void clearStack();
    
    //! Dump the stack's content
    /*!
        See TinyJS documentation
     */
    static void js_dumpStack(CScriptVar *v, void *userdata);
    
    //! print() method
    /*!
     See TinyJS documentation
     */
    static void js_print(CScriptVar *v, void *userdata);
    
    //! internal Callback for selectors & registered methods
    /*!
        This will make a call to JSMachineDelegate's functionCalled() method
     */
    static void js_nativeFunctWithID(CScriptVar *v, void *userdata);
    
//    static void js_readDataFile( CScriptVar *v, void *userdata );
//    static void js_readDir( CScriptVar *v, void *userdata );
    static void js_system( CScriptVar *v, void *userdata );

    
    static void js_importFile( CScriptVar *v, void *userdata );
    
    static void js_FileRead( CScriptVar *v, void *userdata );
    static void js_FileWrite( CScriptVar *v, void *userdata );
    static void js_parseJSON( CScriptVar *v, void *userdata );
    static void js_parseJSONFile( CScriptVar *v, void *userdata );

    static void js_FileExists( CScriptVar *v, void *userdata );
    static void js_fileGetSize( CScriptVar *v, void *userdata );
    static void js_typeof( CScriptVar *v, void *userdata );
    static void js_toFixed(CScriptVar *v, void *userdata);
    
    static void js_getChildren(CScriptVar *v, void *userdata);
    static void js_getObjectAt(CScriptVar *v, void *userdata);
private:
    
#ifdef USE_JSON_PARSER
    static bool jsonParseRecursive( const JSON::Node &node , CScriptVar *var);

#endif
    
    //! remove all selectors from the look-up table
    void removeAllRegisteredFunctions();
    
    void removeAllImportedScripts();
    
    bool findSelectorBySignature( const std::string &signature );
    
    bool _allowSystemCall;
    
    JSMachineDelegate *_delegate;
    
    CTinyJS _machine;
    
    std::vector< Selector* > _registeredSelectors;
    
    std::vector< std::string > _importedHeaders;
    
    
    
    

};

class JSVector : public Element
{
public:
    JSVector()
    {
        setElementName("JSVector");
    }
    ~JSVector()
    {
        _list.clear();
    }
    
private:
#ifdef ENABLE_ELEMENT_SELECTOR
    const Variant performSelectorWithArguments( const std::string &selector , const VariantList  &arguments , bool *validCall = nullptr)
    {
        if( validCall )
            *validCall = true;
        
        if( selector == "clear")
        {
            _list.clear();
            return Variant::null();
        }
        else if( selector == "size")
        {
            return Variant( _list.size() );
        }
        else if( selector == "empty")
        {
            return Variant(_list.empty());
        }
        else if( selector == "at")
        {
            
            return _list.at( static_cast<size_t>( arguments.at(0).getInt() ));
        }
        else if( selector == "find")
        {
            for (const Variant &v : _list)
            {
                if ( v == arguments.at(0) )
                    return Variant(true);
            }
            
            return Variant(false);

            /*
            const auto it = std::find(_list.begin(), _list.end(), arguments.at(0) );
            
            if( it != _list.end() )
                return Variant(*it);
            
            return Variant::null();
             */
            
            
        }
        else if( selector == "push_back")
        {
            _list.push_back( arguments.at(0));
            return Variant::null();
        }
        else if( selector == "remove")
        {
            const auto it = std::find(_list.begin(), _list.end(), arguments.at(0) );
            
            if( it != _list.end() )
            {
                _list.erase( it );
                return Variant(true);
            }

            return Variant(false);
        }

        
        if( validCall )
            *validCall = false;
        return Element::performSelectorWithArguments(selector, arguments, validCall);
    }
#endif
    std::vector<Variant> _list;
};



#endif /* defined(__JSMachine__JSMachine__) */
