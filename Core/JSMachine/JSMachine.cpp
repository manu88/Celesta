//
//  JSMachine.cpp
//  JSMachine
//
//  Created by Manuel Deneu on 02/08/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#include "JSMachine.h"
#include <errno.h>
#include <assert.h>
#include <iomanip>
#include <string>
#include <sstream>
#include <stdio.h>
#include <algorithm>
#include <stdlib.h>
#include <unistd.h>
#include <memory>

// for ip stuffs
/* TO REMOVE if compiles succesfully on Unix ( 7 oct 2015)  */
/*
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
*/
/*
// for dirs
#include <dirent.h>

// time
#include <sys/time.h>
#include <time.h>
*/

#include "TinyJS/TinyJS_Functions.h"
#include "TinyJS/TinyJS_MathFunctions.h"

#include "../Internal/FileSystem.h"
#include "../Data/Database.h"

#include "../Log/Log.h"

#include "JSMachineDelegate.h"




/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

Selector::Selector(const std::string &_signature , JSMachineDelegate* _delegate) :
    signature ( _signature ),
    delegate  ( _delegate  )
{

    className = "Selector";
    
    // on enleve les espaces
//    remove_if(signature.begin(), signature.end(), isspace);
    signature.erase( remove_if(signature.begin(), signature.end(), isspace), signature.end());
    
    std::istringstream mainString( signature );
    
    std::getline(  mainString , identifier , '(' );
    

}

Selector::~Selector()
{

}

void Selector::callFunctionwithVars(CScriptVar *vars)
{
    variables = vars;
    delegate->functionCalled( this );
}


bool Selector::operator==( const Selector& rhs )
{
    return  (signature == rhs.signature) ;
}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

JSMachine::JSMachine() :
    _allowSystemCall ( false   ),
    _delegate        ( nullptr )
{
    className = "JSMachine";
    
    prepareEnvironment();
}



/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

JSMachine::~JSMachine()
{
    removeAllRegisteredFunctions();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void JSMachine::prepareEnvironment()
{
    registerFunctions( &_machine );
    registerMathFunctions( &_machine );
    

    
    _machine.addNative("function dump()"                      , &js_dumpStack, this);
    _machine.addNative("function print(text)"                 , &js_print, 0);
    
    _machine.addNative("function system( command )"           , &js_system, this );
    
    _machine.addNative("function FILE.read( filePath)", js_FileRead, 0);
    _machine.addNative("function FILE.write( filePath , mode , buffer) ", js_FileWrite, 0);
    _machine.addNative("function FILE.exists( filePath) ", js_FileExists, 0);
    _machine.addNative("function FILE.getSize(filePath)", js_fileGetSize, 0);
    
    _machine.addNative("function JSON.parse( textBuffer )", js_parseJSON, 0);
    _machine.addNative("function JSON.parseFile( file )", js_parseJSONFile, 0);

    
    _machine.addNative("function typeof( v )", js_typeof, 0);


    _machine.addNative("function getObjectAt(node , num)", js_getObjectAt, 0);
    _machine.addNative("function getChildren(node)", js_getChildren, 0);
    _machine.addNative("function toFixed(val , decimals)", js_toFixed, 0);


}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void JSMachine::addRegisteredSelectors()
{
    for (auto sel : _registeredSelectors )
    {
        try
        {
            _machine.addNative("function " + sel->signature, js_nativeFunctWithID, sel );
        }
        
        catch (CScriptException *e)
        {
            Log::log("ERROR WHILE REGISTERING: %s", e->text.c_str());
            return;
        }
    }
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void JSMachine::reset()
{
    
    clearStack();
    
    
    
    prepareEnvironment();
    
    addRegisteredSelectors();
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool JSMachine::executeBuffer( const std::string &buffer )
{
    try
    {

        _machine.execute(buffer);

        return true;
    }
    
    catch ( CScriptException *e )
    {
        Log::log("ERROR: %s", e->text.c_str());
        

    }
    
    return false;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

CScriptVarLink JSMachine::evaluate( const std::string &buffer)
{
    try
    {
        return _machine.evaluateComplex( buffer );
    }
    
    catch (CScriptException *e)
    {
        Log::log("ERROR: %s", e->text.c_str());
    }
    
    return CScriptVarLink(new CScriptVar()); // undefined
}


std::string JSMachine::evaluateAsString(const std::string &buffer)
{
    std::string ret = DefaultsValues::Undefined;

    try
    {
        ret =  _machine.evaluate( buffer );
    }
    
    catch (CScriptException *e)
    {
        Log::log("%s", e->text.c_str());
    }
    
    catch (...)
    {
        
    }
    


    
    return ret;
}



/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
 
    Parsers
 
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool JSMachine::parseScriptFile( const std::string &filename )
{
    Log::log("opening file %s ", filename.c_str() );
    
    if ( !FileSystem::fileExists( filename ) )
    {
        Log::log("Cannot Open file! '%s'", filename.c_str() );
        return false;
    }

    const std::string buffer = FileSystem::getFileText( filename );
    
    executeBuffer( buffer );


    
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool JSMachine::importScriptFile( const std::string &filename  )
{
    for ( auto i : _importedHeaders )
    {
        if ( i == filename)

            return true;
    }
    
    _importedHeaders.push_back( filename );
    

    if ( !FileSystem::fileExists( filename ))
    {
        return false;
    }

    const std::string buf = FileSystem::getFileText( filename );
    
    executeBuffer( buf );
    
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
 
    Selectors
 
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void JSMachine::registerFunctionWithSignature(const std::string &signature )
{

    Selector* sel = new Selector( signature , _delegate);
    
    if ( findSelectorBySignature( sel->signature ) )
    {
        Log::log("selector '%s' already exists" , sel->signature.c_str());
        delete sel;
    }
    
    else
    {
        try
        {
            _machine.addNative("function " + signature, js_nativeFunctWithID, sel );
        }
        
        catch (CScriptException *e)
        {
            Log::log("ERROR WHILE REGISTERING: %s", e->text.c_str());
            return;
        }
        
        _registeredSelectors.push_back(sel);

    }
    

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool JSMachine::findSelectorBySignature( const std::string &signature )
{

    for (auto sel : _registeredSelectors )
    {
        
        if (  signature == sel->signature  )
        {
            return true;
        }
    }


    
    return false;
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool JSMachine::respondsToSelector(const std::string &signature)
{
    return findSelectorBySignature( signature );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void JSMachine::removeRegisteredFunctionWithSignature(const std::string &signature)
{
    for (auto sel : _registeredSelectors )
    {
        if (  signature == sel->signature  )
        {
            delete sel;
        }
    }
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void JSMachine::removeAllRegisteredFunctions()
{
//    m_registeredSelectors.er
    for (auto it : _registeredSelectors )
    {

        delete it;
    }
    
    _registeredSelectors.clear();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void JSMachine::removeAllImportedScripts()
{
    _importedHeaders.clear();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void JSMachine::clearStack()
{
    removeAllRegisteredFunctions();
    
    removeAllImportedScripts();
    
    _machine.root->unref();
    
    _machine.root = new CScriptVar();
    _machine.root->ref();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
 
    Statics callback from js interpreter
 
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ void JSMachine::js_nativeFunctWithID(CScriptVar *v, void *userdata)
{
    Selector* sel = reinterpret_cast<Selector*>(userdata);

    sel->callFunctionwithVars(v);
    
}
/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*static*/ void JSMachine::js_typeof(CScriptVar *v, void *)
{
    /*



     isNative()

     isNull()
     isBasic()
     */
    std::string ret = JSDataType::Error;
    
    CScriptVar* param = v->getParameter("v");
    
    if( param->isUndefined())
        ret = JSDataType::Undefined;

    else if( param->isInt())
        ret = JSDataType::Int;

    else if( param->isDouble())
        ret = JSDataType::Double;

    else if( param->isString() )
        ret = JSDataType::String;

    else if( param->isFunction() )
        ret = JSDataType::Function;
    
    else if( param->isObject() )
        ret = JSDataType::Object;
    
    else if( param->isArray() )
        ret = JSDataType::Array;
    
    else if( param->isNull() )
    ret = JSDataType::Null;
    
    else
        assert(false);

    
    v->setReturnVar( new CScriptVar( ret ));
}

/*static*/ void JSMachine::js_toFixed(CScriptVar *v, void *)
{
    //toFixed(val , decimals)
    const double val = v->getParameter("val")->getDouble();
    const int decimals = v->getParameter("decimals")->getInt();
    std::stringstream s;
    s << std::fixed << std::setprecision( decimals ) << val;
    
    v->setReturnVar( new CScriptVar( s.str() ));
    

}


/*static*/ void JSMachine::js_getChildren(CScriptVar *v, void *)
{
    v->setReturnVar( new CScriptVar(v->firstChild->var->getChildren()));
}

/*static*/ void JSMachine::js_getObjectAt(CScriptVar *v, void *)
{
    CScriptVarLink* next =  v->firstChild->var->firstChild;
    const int num  = v->getParameter("num")->getInt();
    
    if(num >= v->firstChild->var->getChildren())
        return;
    
    int i = 0;
    while (next)
    {
        if (i == num)
        {
//            printf("OBJECT '%s'" , next->var->getString().c_str() );
            v->setReturnVar(next->var);
            return;
        }
        i++;
        next = next->nextSibling;
    }
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/void JSMachine::js_print(CScriptVar *v, void *)
{
    printf("%s", v->getParameter("text")->getString().c_str() );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ void JSMachine::js_dumpStack(CScriptVar *, void *userdata)
{
    JSMachine* self = reinterpret_cast<JSMachine*>(userdata);
    
    if (self)
    {
        self->_machine.root->trace(">  ");
        
    }
}



/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static void JSMachine::js_readDataFile(CScriptVar *v, void *userdata)
{
    Database data;
    
    data.parseFile( v->getParameter("filePath")->getString().c_str() , '=');
    
    CScriptVar* vars =  new CScriptVar();
    vars->setArray();
    
    for (int i = 0; i<data.getSize(); i++)
    {
        CScriptVar* pair =  new CScriptVar();
        pair->setArray();
        pair->setArrayIndex(0 , new CScriptVar( data.getItemNameAtIndex( i ) ) ); // item name
        pair->setArrayIndex(1 , new CScriptVar( data.getValueAtIndex( i ).getString() ) ); // item value
        
        vars->setArrayIndex(i , pair);
    }
    
    v->setReturnVar( vars );
    
}
 */

/*static*/ void JSMachine::js_system( CScriptVar *v, void *userdata )
{
    JSMachine* self = reinterpret_cast<JSMachine*>(userdata);
    
    if ( self->_allowSystemCall )
        system( v->getParameter("command")->getString().c_str() );
    
    else
        Log::log("access to system() forbidden");
}



/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ void JSMachine::js_importFile( CScriptVar *v, void *userdata )
{
    JSMachine* self = reinterpret_cast<JSMachine*>(userdata);
    
    if ( self )
    {
        // rep de travail
        self->importScriptFile( v->getParameter("file")->getString().c_str() );

    }
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ void JSMachine::js_FileRead( CScriptVar *v, void * )
{
    //filePath
    v->setReturnVar( new CScriptVar(
                                    FileSystem::getFileText( v->getParameter("filePath")->getString() )
                                    ));
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ void JSMachine::js_FileWrite( CScriptVar *v, void * )
{
    //filePath , mode , buffer
    
    const std::string &mode = v->getParameter("mode")->getString();
    const std::string &file = v->getParameter("filePath")->getString();
    const std::string &buff = v->getParameter("buffer")->getString();

    bool ret = false;
    if( mode == "a")
    {
        ret = FileSystem::appendTextToFile(file , buff );
    }
    else if( mode == "w")
    {
        ret = FileSystem::setFileText( file, buff );
    }
    else
        Log::log("Error while writting file '%s' unrecognized mode '%s'" , file.c_str() , mode.c_str()  );
    
    v->setReturnVar( new CScriptVar( ret ));
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ void JSMachine::js_parseJSON( CScriptVar *v, void * )
{
    const std::string txtBuf = v->getParameter("textBuffer")->getString();
    
    JSON::Document doc;
    if(doc.parseBuffer( txtBuf.c_str() ))
    {
        v->setReturnVar(  JSonParse( doc.getRootNode() ) );
    }
    
}

/*static*/ void JSMachine::js_parseJSONFile( CScriptVar *v, void * )
{
    const std::string file = v->getParameter("file")->getString();
    JSON::Document doc;
    if(doc.parseFile( file ))
    {
        v->setReturnVar( JSonParse( doc.getRootNode() ) );
    }
}


/*static*/void JSMachine::js_FileExists( CScriptVar *v, void * )
{
    v->setReturnVar( new CScriptVar( FileSystem::fileExists( v->getParameter("filePath")->getString() ) ));
}
/*static*/void JSMachine::js_fileGetSize( CScriptVar *v, void * )
{
    v->setReturnVar( new CScriptVar(
                                    static_cast<double>( FileSystem::getFileSize( v->getParameter("filePath")->getString() ))
                                    ));
}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ VariantList JSMachine::getArgumentsFromJSArray( CScriptVar *vars )
{
    VariantList ret;
    
    const int size  = vars->getArrayLength();
    
    // pas un array, mais une seule variable
    if ( size == 0)
    {
        if (vars->isInt() )
            ret.push_back( Variant ( vars->getInt() ) );
        
        else if (vars->isDouble() )
            ret.push_back( Variant ( vars->getDouble() ) );
        
        else if (vars->isString() )
            ret.push_back( Variant ( vars->getString() ) );
        
    }
    
    else
    {
        for (int i = 0; i<size ; i++)
        {
            CScriptVar *var =vars->getArrayIndex( i );
            
            if (var->isInt() )
                ret.push_back( Variant ( var->getInt() ) );
            
            else if (var->isDouble() )
                ret.push_back( Variant ( var->getDouble() ) );
            
            else if (var->isString() )
                ret.push_back( Variant ( var->getString() ) );
        }
    }
    
    return ret;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

std::string JSMachine::getArgumentsAsStringList( const VariantList &list )
{
    std::ostringstream stream;
    
    int i = 0;

    for (const Variant &val : list )
    {
        if ( i>0)
            stream << " , ";
        
        if ( val.isInt() )
        {
            stream << val.getInt();
        }
        else if (val.isInt64())
        {
            stream << val.getInt64();
        }
        else if( val.isUInt64())
            stream << val.getUInt64();
        
        else if ( val.isFloat() )
        {
            stream << val.getFloat();
        }
        else if ( val.isLong() )
        {
            stream << val.getLong();
        }
        else if( val.isULong())
            stream << val.getULong();

        
        else if ( val.isString() )
        {
            stream << "\"" << val.getString() << "\"" ;
        }
        else if( val.isList() )
        {
            stream << "[" << getArgumentsAsStringList(val.getList() ) <<"]";
        }
        else
        {
            /* No parsing method for this type, to be added ...*/
            assert(false);
        }
        
        i++;

    }
    

    
    return stream.str();

}

/*static*/ std::string JSMachine::getCallFromSelector( const std::string &selector , const VariantList  &arguments )
{
    std::string call = selector +"(";
    

    call+= getArgumentsAsStringList(arguments);
    
    call += ");";
    
    return call;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

std::string JSMachine::getArgumentsAsJSArrayString( const VariantList &list )
{
    std::ostringstream stream;
    
    stream << " [ ";
    int i = 0;
    
    for (const Variant &val : list )
    {
        if ( i>0)
            stream << " , ";
        
        if ( val.isInt() )
        {

            stream << val.getInt() ;
        }
        else if ( val.isFloat() )
        {
            stream << val.getFloat();
        }
        
        else if ( val.isString() )
        {
            stream << "\"" << val.getString() << "\"" ;
        }
        
        else
        {
            /* No parsing method for this type, to be added ...*/
            DEBUG_ASSERT(false);
        }
        
        
    }
    
    stream << " ] ";
    
    return stream.str();
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

CScriptVar* JSMachine::getArgumentsAsJSArray( const VariantList &list )
{
    CScriptVar *ret = new CScriptVar();
    ret->setArray();
    
    int count = 0;
    
    for (const Variant &val : list )
    {
        
        if ( val.isInt() || val.isBool() )
        {
            ret->setArrayIndex(count, new CScriptVar( val.getInt() ) );
            count ++;
        }
        else if ( val.isFloat() )
        {
            ret->setArrayIndex(count, new CScriptVar( val.getFloat() ) );
            count ++;
        }
        
        else if( val.isDouble() )
        {
            ret->setArrayIndex( count , new CScriptVar( val.getDouble() ));
            count++;
        }
        
        else if ( val.isString() )
        {
            ret->setArrayIndex(count, new CScriptVar( val.getString() ) );
            count ++;
        }
        
        else if (val.isList() )
        {
            for ( const Variant & vv : val.getList() )
            {
                ret->setArrayIndex(count,  getArgumentsAsJSArray( vv.getList() ));
                count++;
            }
        }
        
        else if ( !val.isNull() )
        {
            /* No parsing method for this type, to be added ... */
            assert(false);
        }
    
    }
    
    return ret;
}


/*static*/ CScriptVar* JSMachine::getArguments( const Variant *var)
{
    
    if (var->isString() )
        return new CScriptVar( var->getString() );
    
    else if (var->isInt() || var->isBool() )
        return new CScriptVar( var->getInt() );
    
    else if (var->isFloat() )
        return new CScriptVar( var->getFloat() );
    
    else if (var->isDouble() )
        return new CScriptVar( var->getDouble() );
    
    else if( var->isLong() )
        return new CScriptVar( static_cast<double>( var->getLong() ) );
    
    else if( var->isULong() )
        return new CScriptVar( static_cast<double>( var->getULong() ) );
    
    else if (var->isInt64())
    {
        return new CScriptVar( static_cast<double>( var->getInt64() ));
    }
    else if (var->isUInt64())
    {
        return new CScriptVar( static_cast<double>( var->getUInt64() ));
    }
    else if( var->isList() )
    {
        return getArgumentsAsJSArray( var->getList() );
    }

    else if ( !var->isNull())
    {
        
//        assert( false);
    }
    
    return new CScriptVar();
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

#ifdef USE_JSON_PARSER

/*static*/ CScriptVar* JSMachine::JSonParse( const JSON::Node & node)
{
    CScriptVar *ret = new CScriptVar();


    jsonParseRecursive( node, ret );
    
    

    
    return ret;
}

/*static*/ bool JSMachine::jsonParseRecursive( const JSON::Node &node , CScriptVar *var)
{
    bool ret = false;
    
    for ( const JSON::Node& nd  : node )
    {

        const std::string name = nd.getName();
        
        
        if( nd.isNumber())
        {
            var->addChild(name , new CScriptVar( nd.getDouble() ) );
        }
        
        else if( nd.isString() )
        {
            var->addChild(name , new CScriptVar( nd.getString() ));
        }
        else if( nd.isObject() )
        {
            jsonParseRecursive( nd , var->getParameter(name));
        }
        else if( nd.isArray() )
        {

            var->getParameter(name)->setArray();
            
            int i = 0;
            for ( const JSON::Node& ndd  : nd )
            {
                if( ndd.isString() )
                {
                    var->getParameter(name)->setArrayIndex(i, new CScriptVar(ndd.getString() ) );
                }
                else if( ndd.isNumber())
                    var->getParameter(name)->setArrayIndex(i, new CScriptVar(ndd.getDouble() ));
                
                else if( ndd.isObject() )
                {
                    CScriptVar *varr = new CScriptVar();
                    jsonParseRecursive(ndd, varr);
                    var->getParameter(name)->setArrayIndex(i, varr);
                    
                }
                else
                {
                    printf(" Node name %s is type %i \n" , ndd.getName().c_str() , ndd.getType() );
                    
                }
                
                i++;
                
            }
        }

    }

    
    
    return ret;
}

#endif


