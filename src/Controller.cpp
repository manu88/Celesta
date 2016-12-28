//
//  Controller.cpp
//  JSCenter
//
//  Created by Manuel Deneu on 08/07/2015.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#include "Controller.h"
#include "../Core/Env_Variables.h"



Controller::Controller( const std::string &configFile):
ApplicationBase( configFile )
{
    setElementName("JSController");

    _js.allowSystemCalls( true );
    

}

Controller::~Controller()
{
   
}

bool Controller::applicationWillStart()
{
    

    _js.setDelegate( this );
    
    

    reload();

    return true;
}

void Controller::applicationDidStart()
{
    assert( getRunLoop().calledFromThisRunLoop());
    
    //assert( Dispatch::isTheMainThread() );

    if( _scriptFile.empty() && dataValueExists("ScriptFile") )
       _scriptFile = getDataValue("ScriptFile", Variant("") )->getString() ;
        
    
    if( _js.parseScriptFile( _scriptFile ) )
        _js.executeBuffer("applicationDidStart();");
    
    
}

void Controller::applicationWillStop()
{
    assert( getRunLoop().calledFromThisRunLoop());
    
    _js.executeBuffer("applicationWillStop();");
    
}


void Controller::applicationDidStop()
{
    assert( getRunLoop().calledFromThisRunLoop());

    
    for (const Element* child : Element::getChildren() )
    {
        delete child;
    }
    
    Controllers::waitForAllControllersToFinish();

    GXFont::deleteLoadedFonts();

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */


bool Controller::reload()
{
    //assert( Dispatch::isTheMainThread() );
    
    for( const Element *child : Element::getChildren() )
    {
        delete child;
    }

    _js.reset();
    
    _js.registerFunctionWithSignature("import( file )");
    
    /* App base */
    
    _js.registerFunctionWithSignature("APP_NAME()");
    
    _js.registerFunctionWithSignature("reload()");
    _js.registerFunctionWithSignature("load( fileScript )");
    
    _js.registerFunctionWithSignature("delay(ms)");
    _js.registerFunctionWithSignature("log(text)");
    
    /* Timers */
    
    _js.registerFunctionWithSignature("addTimer ( delay , period , fct)");
    _js.registerFunctionWithSignature("removeTimer(id)");
    _js.registerFunctionWithSignature("timerExists(id)");
    _js.registerFunctionWithSignature("timerSetFlexible( id )");
    _js.registerFunctionWithSignature("timerGetIdentifier( id)");

    /* GPIO */
    
    _js.registerFunctionWithSignature("getBytesFromStr( str)");
    
    /* Callbacks fcts */
    
    _js.registerFunctionWithSignature("setOscCallback( fct )");
    _js.registerFunctionWithSignature("setInterfaceCallback( fct )");
    _js.registerFunctionWithSignature("setMessageCallback( fct )");
    _js.registerFunctionWithSignature("setWebCallback( fct)");
    _js.registerFunctionWithSignature("setDisplayCallback( fct )");
    _js.registerFunctionWithSignature("setSerialCallback( fct )");
    
    
    /* Dispatch */
    
    _js.registerFunctionWithSignature("postMessage(msg)");
    _js.registerFunctionWithSignature("quit()");
    _js.registerFunctionWithSignature("perform( selector , object , variables) ");
    _js.registerFunctionWithSignature("performStatic( selector , className , variables)");
    _js.registerFunctionWithSignature("performAsync(func)");
    
    /* Factory */
    
    _js.registerFunctionWithSignature("createElement( name )");
    _js.registerFunctionWithSignature("deleteElement( id )");
    
    
    _js.registerFunctionWithSignature("getFileManager()");    
    /* Factory */
    _js.registerFunctionWithSignature("loadFont( fileName )");
    _js.registerFunctionWithSignature("loadFontDir( path )");
    _js.registerFunctionWithSignature("listLoadedFonts()");
    
    /* Files, dirs */
    
    _js.registerFunctionWithSignature("getDirContent( path , withFullPath)");
    
    
//  deprec.  _js.registerFunctionWithSignature("fileExists( path )");
//  deprec.  _js.registerFunctionWithSignature("dirExists( path )");
    
    /* Geometry */
    
    _js.registerFunctionWithSignature("rectIntersectsRect( rectArray0 , rectArray1) ");
    _js.registerFunctionWithSignature("rectContainsPoint( rectArray0 , pointArray1) ");
    /**/
    
    /* Init classes */
    _js.registerFunctionWithSignature("InitClass( className )");
    

    return true;
}



void Controller::functionCalled( const Selector *selector )
{
    
}

void Controller::onTime( const Timer &timer)
{
    assert( getRunLoop().calledFromThisRunLoop());
    
    printf("On time \n");
}

