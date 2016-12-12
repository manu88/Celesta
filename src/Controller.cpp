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
ApplicationBase( configFile ),

_msgCallback       ( "" ),
_netCallback       ( "" ),
_interfaceCallback ( "" ),
_displayCallback   ( "" )
{
    setElementName("JSController");

    _js.allowSystemCalls( true );
    
    //Dispatch::getInstance()->addObserverForAttribute( this, ElementAttributes::State);

}

Controller::~Controller()
{
   // Dispatch::getInstance()->removeAttributeForObserver(this, ElementAttributes::State );
}

bool Controller::applicationWillStart()
{
    assert( Dispatch::isTheMainThread() );
    assert( _timers.empty() );

    _js.setDelegate( this );
    
    Dispatch::getInstance()->addListener( this );

    reload();

    return true;
}

void Controller::applicationDidStart()
{
    assert( Dispatch::isTheMainThread() );

    if( _scriptFile.empty() && dataValueExists("ScriptFile") )
       _scriptFile = getDataValue("ScriptFile", Variant("") )->getString() ;
        
    
    if( _js.parseScriptFile( _scriptFile ) )
        _js.executeBuffer("applicationDidStart();");
    
    
}

void Controller::applicationWillStop()
{
    assert( Dispatch::isTheMainThread() );
    
    _js.executeBuffer("applicationWillStop();");
    
}


void Controller::applicationDidStop()
{
    assert( Dispatch::isTheMainThread() );

    for (auto timers : _timers)
        delete timers;
    
    _timers.clear();

    for (const Element* child : Element::getChildren() )
    {
        delete child;
    }
    
    Controllers::waitForAllControllersToFinish();

    GXFont::deleteLoadedFonts();

}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void Controller::viewWillAppear   ( UIView *view )
{
    Dispatch::getInstance()->performAsync(this, this, "viewWillAppear", {Variant( view->getElementId() )} );
}
void Controller::viewDidAppear    ( UIView *view )
{
    Dispatch::getInstance()->performAsync(this, this, "viewDidAppear", {Variant( view->getElementId() )} );
}
void Controller::viewWillDisappear( UIView *view )
{
    Dispatch::getInstance()->performAsync(this, this, "viewWillDisappear", {Variant( view->getElementId() )} );
}
void Controller::viewDidDisappear ( UIView *view)
{
    Dispatch::getInstance()->performAsync(this, this, "viewDidDisappear", {Variant( view->getElementId() )} );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */


bool Controller::reload()
{
    assert( Dispatch::isTheMainThread() );
    
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


Element* Controller::createElement( const std::string &name)
{
    Element *e = nullptr;
    
    if( name == ClassNames::GXVideo )
    {
        e = new GXVideo();
    }
    else if( name == ClassNames::GXImage)
    {
        e = new GXImage();
    }
    else if( name == ClassNames::GXAsyncPainter)
    {
        e = new GXAsyncPainter();
    }
    else if( name == ClassNames::SerialEvent)
    {
        e = new SerialEvent();
    }
    else if ( name == ClassNames::JSON_Document )
    {
        e = new JSON::Document();
    }
    else if( name == ClassNames::GXScene)
    {
        e = new GXScene();
    }
    else if ( name == ClassNames::GXTextRenderer)
    {
        e = new GXTextRenderer();
    }
    else if( name =="WaitComponent")
    {
        e = new CircleWaitComponent();
    }
    else if (name == ClassNames::Chrono)
    {
        e = new Chrono();
    }
    else if( name == ClassNames::WebServer )
    {
        e = new WebServer();
        (dynamic_cast<WebServer*>( e ) )->setDelegate( this );
    }
    else if (name == ClassNames::Url)
    {
        Url *url = new Url();
        url->setTempPath( getTempDirectory()  );
        e = url;
    }
    
    else if (name == ClassNames::DisplayController )
    {
        e = DisplayController::getInstance();
        
        DisplayController::getInstance()->setParentElement(this);
        
        DisplayController::getInstance()->setAsyncCallback( true );
        
        Dispatch::getInstance()->add( DisplayController::getInstance() );
    }
    
    else if( name == ClassNames::InterfaceController )
    {
        e = new InterfaceController();
        (dynamic_cast<InterfaceController*>( e ) )->setDelegate( this );
        
        Dispatch::getInstance()->add( (dynamic_cast<InterfaceController*>( e ) ) );
    }
    
    else if( name == ClassNames::GXTouchController)
    {
        e = GXTouchController::getInstance();
        (dynamic_cast<GXTouchController*>( e ) )->setDelegate( this );
        
        Dispatch::getInstance()->add( (dynamic_cast<GXTouchController*>( e ) ) );
    }
    

    else if( name == ClassNames::NetworkController )
    {
        e = new NetworkController();
        (dynamic_cast<NetworkController*>( e ) )->setAsyncCallback( true );
        Dispatch::getInstance()->add( (dynamic_cast<NetworkController*>( e ) ) );
    }
    else if (name == ClassNames::IPCListener )
    {
        e = new IPCListener();
    }
    else if (name == ClassNames::IPCSender )
    {
        e = new IPCSender();
    }
    
    else if( name == ClassNames::UIWindow )
    {
        e = new UIWindow();
    }
    
    else if( name == ClassNames::UIButton)
    {
        e = new UIButton();
    }
    else if( name == ClassNames::UILabel)
    {
        e = new UILabel();
    }
    else if( name == ClassNames::UISlider)
    {
        e = new UISlider();
    }
    else if( name == ClassNames::UIImage)
    {
        e = new UIImage();
    }
    else if( name == ClassNames::UIStoryBoard)
    {
        e = new UIStoryBoard();
    }
    else if( name == ClassNames::UIVideo)
    {
        e = new UIVideo();
    }
    
    else if( name == "Vector")
    {
        e = new JSVector();
    }
    
    /**/
    
    if( e )
    {
        e->setParentElement( this );
//        e->addObserverForAttribute( this , UIAttribute::Children );
    }

    return e;
}

void Controller::initElementClass( const std::string &className )
{

    if( className == ClassNames::GXAnimation )
        GXAnimation::init();
    
    else if (className == ClassNames::Element)
        Element::init();
        
    
    else if( className == ClassNames::GXElement)
        GXElement::init();
    
    else if( className == ClassNames::WebServer)
        WebServer::init();
    
    else if( className == ClassNames::Url )
        Url::init();
    
    else if( className == ClassNames::UIModalAlert )
        UIModalAlert::init();
}

void Controller::postMessage( const std::string &msg)
{
    assert( Dispatch::isTheMainThread() );
    
    EventMessage* evt = new EventMessage( msg , this );

    Dispatch::getInstance()->pushNotification( evt );
}

void Controller::functionCalled( const Selector *selector )
{
    assert( Dispatch::isTheMainThread() );

    if( selector->identifier == "quit")
    {
        sendQuitSignal();
        return;
    }

    else if ( selector->identifier == "reload" )
    {
        restart();
        return;
    }
    else if( selector->identifier == "load")
    {
        const std::string script = selector->variables->getParameter("fileScript")->getString();
        

    }
    
    else if ( selector->identifier == "postMessage")
    {
        postMessage( selector->variables->getParameter("msg")->getString() );
        return;
    }
    
    else if( selector->identifier == "performAsync")
    {
        
        printf("Async : '%s' \n" , selector->variables->firstChild->var->getString().c_str());
        return;
    }
    
    else if( selector->identifier == "perform")
    {
        const std::string select = selector->variables->getParameter("selector")->getString();

        const VariantList vars   = Variant::createList( selector->variables->getParameter("variables") );
        
        std::string obj    = selector->variables->getParameter("object")->getString();

        const Element* el = nullptr;
        
        if ( selector->variables->getParameter("object")->isNumeric() )
        {
            el = Element::getElementById( static_cast<element_id>(selector->variables->getParameter("object")->getInt() ) );
            
            if (el != nullptr )
                obj = el->getElementName();
        }
        

        bool validObj  = false;
        bool validCall = false;
        const Variant &ret = Element::performSelectorOnElement( obj,select, vars , &validCall , &validObj);
        

        if (!validObj)
        Log::log("Error : invalid obj '%s' (class %s)" , obj.c_str() ,el? el->getClassName().c_str() : "UNKNOWN" );
        
        if( !validCall)
        {
            Log::log("Error : invalid call ");
            Log::log("perform Selector '%s'" , select.c_str() );
            Log::log("on obj '%s' (class %s)" , obj.c_str() , el? el->getClassName().c_str() : "UNKNOWN" );
            Log::log("With args : ");
            
            for (const auto &v : vars)
                Log::log("'%s'",v.getString().c_str() );

            Log::log("\n");
        }


        selector->variables->setReturnVar( JSMachine::getArguments( &ret ) );
        return;
    }
    
    else if( selector->identifier == "performStatic")
    {
        //( selector , className , variables
        const std::string select = selector->variables->getParameter("selector")->getString();
        const std::string className = selector->variables->getParameter("className")->getString();
       
        const VariantList vars   = Variant::createList( selector->variables->getParameter("variables") );
        
        bool validClass = false;
        bool validCall  = false;
        
        const Variant &ret = Element::performStaticSelectorWithArguments(className, select , vars , &validCall , &validClass );
        
        selector->variables->setReturnVar( JSMachine::getArguments( &ret ) );
        return;
    }
    
    /* **** **** **** **** **** **** **** **** **** **** **** **** */
    
    else if ( selector->identifier == "setOscCallback")
    {
        _netCallback = selector->variables->getParameter("fct")->getString();
        return;
    }
    
    else if ( selector->identifier == "setInterfaceCallback")
    {
        _interfaceCallback = selector->variables->getParameter("fct")->getString();
        return;
    }
    
    else if ( selector->identifier == "setMessageCallback")
    {
        _msgCallback = selector->variables->getParameter("fct")->getString();
        return;
    }
    else if ( selector->identifier == "setWebCallback")
    {
        _webCallback = selector->variables->getParameter("fct")->getString();
        return;
    }
    else if ( selector->identifier == "setDisplayCallback")
    {
        _displayCallback = selector->variables->getParameter("fct")->getString();
        return;
    }
    else if ( selector->identifier == "setSerialCallback")
    {
        _serialCallback = selector->variables->getParameter("fct")->getString();
        return;
    }

    /* **** **** **** **** **** **** **** **** **** **** **** **** */
    
    else if( selector->identifier =="delay")
    {
        Thread::sleepForMs( static_cast<unsigned long>( selector->variables->getParameter("ms")->getInt() ) );
        return;
    }
    
    /* **** **** **** **** **** **** **** **** **** **** **** **** */
    
    else if ( selector->identifier == "addTimer")
    {
        //addTimer ( delay , period , fct)
        const int delay = selector->variables->getParameter("delay")->getInt();
        const int period = selector->variables->getParameter("period")->getInt();
        const std::string fct = selector->variables->getParameter("fct")->getString();
        
        const Dispatch::timer_id id =createTimer( Timecode(0,0,0,static_cast<unsigned long>( delay  )),
                                                  Timecode(0,0,0,static_cast<unsigned long>( period )),
                                                  fct );
        
        selector->variables->setReturnVar( new CScriptVar( (int) id  ) );
        return;
    }
    
    else if ( selector->identifier == "removeTimer")
    {
        //addTimer ( delay , period , fct)
        const int timerId = selector->variables->getParameter("id")->getInt();

        selector->variables->setReturnVar(
                                          new CScriptVar(
                                                          removeTimer ( static_cast< Dispatch::timer_id>( timerId ))
                                                        )
                                          );
        return;
    }
    
    else if ( selector->identifier == "timerExists")
    {
        const Dispatch::timer_id timerId =static_cast< Dispatch::timer_id>( selector->variables->getParameter("id")->getInt() );
        
        const bool exists =  Dispatch::getInstance()->exists( timerId ) ;
        
        selector->variables->setReturnVar( new CScriptVar( exists ) );
        return;
    }
    
    else if (selector->identifier == "timerSetFlexible" )
    {
        const Dispatch::timer_id timerId = static_cast< Dispatch::timer_id>(selector->variables->getParameter("id")->getInt() );
        
        const bool ret = Dispatch::getInstance()->timerSetFlexible( timerId );
        
        selector->variables->setReturnVar( new CScriptVar( ret ) );
        
        return;
    }
    
    else if( selector->identifier == "timerGetIdentifier")
    {
        const Dispatch::timer_id timerId = static_cast< Dispatch::timer_id>(selector->variables->getParameter("id")->getInt() );
        
        selector->variables->setReturnVar( new CScriptVar( Dispatch::getInstance()->timerGetIdentifier( timerId ) ));
        
        return;
    }

    /* **** **** **** **** **** **** **** **** **** **** **** **** */
    
    else if ( selector->identifier =="getFileManager")
    {
        selector->variables->setReturnVar( new CScriptVar( (int) getFileManager()->getElementId() ));
        return;
    }
    
    else if ( selector->identifier =="getDirContent" )
    {
        const auto path     = FileSystem::correctPathIfNeeded(selector->variables->getParameter("path")->getString()) ;
        const bool fullPath = selector->variables->getParameter("withFullPath")->getBool();
        
        
        CScriptVar* ret = selector->variables->getReturnVar();
        ret->setArray();
        int i = 0;
        for (const std::string &file : FileSystem::getFilesListFromFolder(path, fullPath ))
        {
            ret->setArrayIndex( i++, new CScriptVar( file ) );
        }

        return;
    }
    
    /* **** **** **** **** **** **** **** **** **** **** **** **** */
    
    else if (selector->identifier == "createElement")
    {
        const auto name = selector->variables->getParameter("name")->getString();
        
        Element* element = createElement( name );
        
        if (element)
            selector->variables->setReturnVar( new CScriptVar((int) element->getElementId() )) ;
        else
            selector->variables->setReturnVar( new CScriptVar( -1 ));
        
        return;
    }
    
    else if (selector->identifier == "deleteElement")
    {
        const element_id id = static_cast<element_id>( selector->variables->getParameter("id")->getInt() );
        
        selector->variables->setReturnVar( new CScriptVar( deleteElement( id) ) );
        return;
    }
    
    /* **** **** **** **** **** **** **** **** **** **** **** **** */
    
    else if (selector->identifier == "loadFont")
    {
        const std::string file = selector->variables->getParameter("fileName")->getString();
        
        selector->variables->setReturnVar( new CScriptVar( GXFont::loadFont(file) != nullptr ));
        return;
    }
    
    else if ( selector->identifier == "loadFontDir")
    {
        const std::string path = selector->variables->getParameter("path")->getString();
        
        if( !FileSystem::folderExists(path) )
            selector->variables->setReturnVar( new CScriptVar( false ));
        else
        {
            for ( const std::string &file : FileSystem::getFilesListFromFolder( path, true) )
            {
                if( GXFont::loadFont( file ) == nullptr )
                    Log::log("Error loading font '%s'" , file.c_str() );
                
            }
            selector->variables->setReturnVar( new CScriptVar( true ));
        }

        
        return;
    }
    
    else if ( selector->identifier == "listLoadedFonts")
    {
        CScriptVar *listRet = new CScriptVar();
        listRet->setArray();
        
        int i = 0;
        for (const GXFont *font : GXFont::getFontAtlas() )
        {
            listRet->setArrayIndex(i++, new CScriptVar( font->getName() ));
        }
        
        selector->variables->setReturnVar( listRet );
        return;
    }

    /* **** **** **** **** **** **** **** **** **** **** **** **** */
    
    else if ( selector->identifier == "import")
    {
        /* Test JS Folder */
        const std::string headerFile = getDataValue( SystemFolders::JsHeaders, Variant("") )->getString() + selector->variables->getParameter("file")->getString();
                
        if (!_js.importScriptFile( headerFile ))
        {
            /* Test in working dir. */
            
            const std::string file = FileSystem::getPathLocation(getDataValue("ScriptFile", Variant("") )->getString() ) +
                                        selector->variables->getParameter("file")->getString();
            
            if (!_js.importScriptFile( file) )
                Log::log("Error while importing file '%s' " , headerFile.c_str() );
        }
        return;
    }
    
    else if (selector->identifier == "getBytesFromStr")
    {
        const Variant r (selector->variables->getParameter("str")->getString() );
        
        CScriptVar *s = new CScriptVar();
        s->setArray();
        
        int i = 0;
        
        for (const uint8_t &v : r.getByteArray())
            s->setArrayIndex(i++, new CScriptVar( v) );
        
        selector->variables->setReturnVar( s);
        return;
    }
    
    /* **** **** **** **** **** **** **** **** **** **** **** **** */
    /*
        Geometry
     */
    /* **** **** **** **** **** **** **** **** **** **** **** **** */
    
    else if( selector->identifier =="rectIntersectsRect" )
    {
        const GXRect r0 = makeRect( selector->variables->getParameter("rectArray0")->getArrayIndex(0)->getInt(),
                                    selector->variables->getParameter("rectArray0")->getArrayIndex(1)->getInt(),
                                    selector->variables->getParameter("rectArray0")->getArrayIndex(2)->getInt(),
                                    selector->variables->getParameter("rectArray0")->getArrayIndex(3)->getInt()
                                   );
        
        const GXRect r1 = makeRect( selector->variables->getParameter("rectArray1")->getArrayIndex(0)->getInt(),
                                   selector->variables->getParameter("rectArray1")->getArrayIndex(1)->getInt(),
                                   selector->variables->getParameter("rectArray1")->getArrayIndex(2)->getInt(),
                                   selector->variables->getParameter("rectArray1")->getArrayIndex(3)->getInt()
                                   );
        selector->variables->setReturnVar( new CScriptVar( rectIntersectsRect(r0, r1) ));
        return;
    }
    
    else if ( selector->identifier =="rectContainsPoint")
    {
        //( rectArray0 , pointArray1))
        const GXRect r = makeRect( selector->variables->getParameter("rectArray0")->getArrayIndex(0)->getInt(),
                                   selector->variables->getParameter("rectArray0")->getArrayIndex(1)->getInt(),
                                   selector->variables->getParameter("rectArray0")->getArrayIndex(2)->getInt(),
                                   selector->variables->getParameter("rectArray0")->getArrayIndex(3)->getInt()
                                   );
        
        const GXPoint p = makePoint( selector->variables->getParameter("pointArray1")->getArrayIndex(0)->getInt(),
                                     selector->variables->getParameter("pointArray1")->getArrayIndex(0)->getInt()
                                    );
        selector->variables->setReturnVar( new CScriptVar( rectContainsPoint(r, p) ));
        return;
        
    }
    /*GXAnimationInit()
     GXElementInit()")*/
    else if (selector->identifier == "InitClass")
    {
        
        initElementClass( selector->variables->getParameter("className")->getString() );
        return;
    }
    
    else if( selector->identifier == "APP_NAME")
    {
        selector->variables->setReturnVar( new CScriptVar( getApplicationName() ));
        assert(false);
        return;
    }
    


    assert( false );
}


void Controller::oscReceived(const std::string &ipAddress , const int port,const std::string & addressPattern, const VariantList &arguments)
{
    assert( Dispatch::isTheMainThread() );
    
    if (addressPattern == "/perform")
    {
        Log::log("\n perform ret '%s'",     Element::performSelectorOnElement(arguments.at(0).getString(),
                                                                            arguments.at(1).getString(),
                                                                            VariantList(arguments.begin()+2 , arguments.end() )
                                          ).getString().c_str() );
    }

    else
    {
        std::stringstream s;
        
        s << _netCallback  <<  "(" << "\"" << ipAddress<< "\"" << " , " << port << " , " << "\"" <<  addressPattern << "\"" << "," ;
        
        if( !arguments.empty())
        {
            s << "[";
            bool first = true;
            for ( const auto &v : arguments)
            {
                if (!first)
                    s << "," ;//v;
                else
                    first = false;

                if (v.isString() )
                    s << "\"" << v << "\"" ;
                else
                    s <<  v ;
            }
            
            s << "]";
            
        }
        else
            s << DefaultsValues::Undefined;
        
        s << ");";
        
        _js.executeBuffer(s.str() );
    }

}

void Controller::inputChanged( const InterfaceEvent *event )
{
    assert( Dispatch::isTheMainThread() );
    
    if ( event->getType() == Event_Gpio )
    {
        const GpioEvent* gp = Event::cast<const GpioEvent*>(event );
        
        if( !_interfaceCallback.empty() )
        {
            std::stringstream s;
            
            s << _interfaceCallback << "(" << gp->getPin() << " , " << (int)gp->getState() << ");";
            
            _js.executeBuffer(s.str() );
        }
    }
    else if( event->getType() == Event_Serial)
    {
        const SerialEvent* s = Event::cast<const SerialEvent*>(event);
        
        if( !_serialCallback.empty() )
            _js.executeBuffer( _serialCallback + "(\"" +  s->readString() + "\");" );        
    }
    

/*
    else if (event->getType() == Event_Touch)
    {
        const TouchEvent *touch = Event::cast<const TouchEvent*>(event);
        
//        printf("touch at %i %i \n" , touch->getX()  , touch->getY() );
        _js.executeBuffer( std::string("touchEvent")+ "(" + std::to_string( touch->getX() ) + "," +
                          std::to_string( touch->getY() )
                          + ");" );
    }
*/
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
void Controller::toucheBegan( const GXPoint &pt )
{
    _js.executeBuffer( std::string("toucheBegan")+ "(" +
                      std::to_string( pt.x ) + "," +
                      std::to_string( pt.y )
                      + ");" );
}

void Controller::toucheMoved( const GXPoint &pt)
{
    _js.executeBuffer( std::string("toucheMoved")+ "(" +
                      std::to_string( pt.x ) + "," +
                      std::to_string( pt.y )
                      + ");" );
}
void Controller::toucheEnded( const GXPoint &pt)
{
    _js.executeBuffer( std::string("toucheEnded")+ "(" +
                      std::to_string( pt.x ) + "," +
                      std::to_string( pt.y )
                      + ");" );
}
*/
/* **** **** **** **** **** **** **** **** **** **** **** **** **** */


void Controller::eventReceived( const Event *event)
{
    assert( Dispatch::isTheMainThread() );
    
    if (event->getType() == Event_Message )
    {
        const EventMessage *msg = Event::cast<const EventMessage*>(event);

        if ( !_msgCallback.empty())
        {
            const std::string ret = _msgCallback+"("+std::to_string(event->getSender()->getElementId() )+","+"\""+msg->message+"\");" ;
            _js.executeBuffer( ret );
        }
    }
    
    /* **** */
    
    else if (event->getType() == Event_Video )
    {
        const VideoEvent *vid = Event::cast<const VideoEvent*>(event);
        if ( !_msgCallback.empty())
        {   
            const std::string ret = _msgCallback+"("+std::to_string(event->getSender()->getElementId() )+","
                                              +std::to_string(vid->notification ) +");" ;
            
            _js.executeBuffer( ret );
        }
    }
    
    /* **** */
    
    else if( event->getType() == Event_OSC )
    {
        const EventOscIn* msg = Event::cast< const EventOscIn*>( event );
        
        oscReceived( msg->ip, msg->port, msg->address, msg->arguments);
        
    }
    
    /* **** */
    
    else if( event->getType() == Event_Display )
    {
        if ( !_displayCallback.empty())
        {
            const EventDisplay* msg = Event::cast<const EventDisplay*>(event);
            
            const std::string ret = _displayCallback+"(\""+ std::to_string((unsigned int)msg->notification)+"\");" ;
            
            _js.executeBuffer( ret );
        }
    }
    
    else if( event->getType() == Event_FileManager )
    {
        Log::log(" Notif from file manager \n");
    }
    else
    {}
}

std::string Controller::getRequest( const std::string &ipAddress ,
                                    const int port,
                                    const std::string &addressPattern,
                                    const Database &arguments
                                   )
{
    
    const std::string dat = JSMachine::getDatabaseAsJSArrayString( arguments );
    

    const std::string ret = _webCallback+"("+ "\"" + ipAddress+"\""          + ","
                                                   + std::to_string(port)    + ","
                                              "\"" + addressPattern+"\""     + ","
                                                   + dat
                                        +");" ;
    
    return _js.evaluateAsString( ret );
    
}

void Controller::TimerHandler::onTime()
{
    _ctlr->onTime( _callback );
}

void Controller::onTime( const std::string &callback)
{
    _js.executeBuffer( callback+"();" );
}

Dispatch::timer_id Controller::createTimer( const Timecode &delay , const Timecode &period , const std::string &callback)
{
    TimerHandler *timer = new TimerHandler( this , callback );
    _timers.push_back( timer );
    return Dispatch::getInstance()->create( delay.getInMs(),
                                            period.getInMs(),
                                            std::bind(&Controller::TimerHandler::onTime , timer ) ,
                                            getApplicationName() + "." + ClassNames::Timer + "." + callback
                                           );

}

bool Controller::removeTimer( const Dispatch::timer_id id )
{
    return Dispatch::getInstance()->destroy( id );
}
