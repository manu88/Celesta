//
//  DisplayController.cpp
//  Broadway_test
//
//  Created by Manuel Deneu on 22/09/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//


#include <assert.h>

#include "DisplayController.h"
#include "../Env_Variables.h"
#include "../Plateforms/Plateform.h"
#include "../Log/Log.h"

#include "../Scheduler/Dispatch.h"

#include "GXPath.h"

DisplayController* DisplayController::s_instance = nullptr;


DisplayController::DisplayController() :
AbstractController   ( ClassNames::DisplayController ),
Thread               ( "GUI"   ),
_impl( this ),
_displayIsOn         ( false ),
_delegate            ( nullptr ),
_asyncCallback       ( false ),

#ifdef ENABLE_ELEMENT_SELECTOR
_notifTarget(nullptr),
#endif

_clearColor ( makeColor( 0,0,0) ),

_currentElement      ( nullptr ),
_nextElement         ( nullptr ),
m_shouldClearContext ( true    ),
_frameRate           ( 0.0 ),
_desiratedFrameRate  ( 60.0 ),

/* Config file load/save */

_shouldSaveConfigOnFile ( false ),
_fileConfig("screenConfig.txt")

{
    setElementName( ClassNames::DisplayController );
    className     = ClassNames::DisplayController ;
    
    addTypeInfo( Output );
    addTypeInfo( Video );   
}

DisplayController::~DisplayController()
{
    stop();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool DisplayController::start()
{
    return startThread();
}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool DisplayController::stop()
{
    _wakeUp.notify_all();
    return stopThread();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void DisplayController::removeDisplayedElement()
{
    if(_currentElement == nullptr)
        return;
    
    ScopedLock lock( _sync );
    lock.unlock();
    
    _currentElement->removeFromParent();
    
    lock.lock();
}

void DisplayController::setDisplayedElement( GXElement* element)
{
    ScopedLock lock( _sync );
    lock.unlock();

    if( _currentElement)
    {
        _currentElement->removeFromParent();
    }
    
    _nextElement = element;

    lock.lock();
    
    _wakeUp.notify_all();
}



/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void DisplayController::setCleanColor( GXColor color)
{
    _clearColor = color;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void DisplayController::clearScreen()
{
    
    m_shouldClearContext = true;
    _wakeUp.notify_all();
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void DisplayController::update()
{
    if (_currentElement)
        _currentElement->setNeedsDisplay();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void DisplayController::run()
{
    if( !_impl.initPlateform() )
        Log::log("Display Plateform initialization failed");
    
    //DisplayInformations mode = _impl.getCurrentDisplayInformations();
    
    if( !_impl.initDisplay() )
        Log::log("Display  initialization failed");
    
    setReady();
    
    clearContext();
    updateContext();
    
    displayChangeNotification( DISPLAY_ACTIVE );
   
    ScopedLock lock( _sync );
    
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

    while ( !threadShouldStop() )
    {
        start = std::chrono::system_clock::now();
        
        if( _nextElement != nullptr )
        {
            if( _currentElement )
            {
                lock.unlock();
                _currentElement->changeCycleTo( GXElement::CycleState::NotVisible);
                lock.lock();
            }
            else if( _currentElement == nullptr )
            {
                _currentElement = _nextElement;
                _nextElement = nullptr;
                
                _currentElement->setBounds( makeRect(makePointNULL(),  getCurrentMode().size) );
                
                lock.unlock();
                _currentElement->changeCycleTo( GXElement::CycleState::WillAppear);
                lock.lock();
                
                if( _currentElement->isPrepared() )
                {
                    lock.unlock();
                    _currentElement->changeCycleTo( GXElement::CycleState::Visible);
                    lock.lock();
                }
                
                _currentElement->setNeedsDisplay();
                
                continue;
            }
        }
        
        if ( _currentElement == nullptr )
        {
            _wakeUp.wait( lock );
        }

        _wakeUp.wait_for( lock , std::chrono::milliseconds((int)(500/_desiratedFrameRate)) );

        if (m_shouldClearContext )
        {
            clearContext();
            updateContext();
            m_shouldClearContext = false ;
        }
        
        else if ( _currentElement )
        {
            if ( _currentElement->shouldBeRemoved() )
            {
                lock.unlock();

                _currentElement->changeCycleTo( GXElement::CycleState::NotVisible);
                _currentElement->_shouldBeRemoved = false;
                
                lock.lock();
                
                _currentElement = nullptr;
                continue;
            }
            
            if ( !_currentElement->isPrepared() )
            {
                lock.unlock();
                _currentElement->prepare();
                
                if( _currentElement->_elementCycleState != GXElement::CycleState::Visible )
                    _currentElement->changeCycleTo( GXElement::CycleState::Visible);
                lock.lock();
            }
            
            if ( _currentElement->_changed_flag && _currentElement->m_callChangedOnGUIThread)
            {
                lock.unlock();
                _currentElement->changed();
                lock.lock();
                
                _currentElement->_changed_flag = false;
            }
            
            else if ( _currentElement->needsDisplay() )
            {
                if ( threadShouldStop() )
                    break;
                
                lock.unlock();
                _currentElement->update( _currentElement->_updateRect , _currentElement->_anim );
                _currentElement->setUpdated();
                lock.lock();
                
                updateContext();
                const std::chrono::duration<double> diff = std::chrono::system_clock::now()-start;
                
                _frameRate = static_cast<float>( 1.0f / diff.count() );
                
                _impl.checkErrors();
                
                if ( threadShouldStop() )
                    break;
            }
        }
    }
    
    if ( _currentElement )
    {
        lock.unlock();
        _currentElement->cleanUp();
        lock.lock();
    }
    
    _impl.deInitDisplay();
    
    setUnReady();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void DisplayController::displayChangeNotification( DisplayNotification notification )
{
    if( _asyncCallback)
    {
        if( _notifTarget)
        {
            _notifTarget->performSelectorWithArguments(_notifSelector, { Variant(notification) } );
        }
        else
            Dispatch::getInstance()->pushNotification( new EventDisplay( notification ,this ) );
    }
    else if ( _delegate )
        _delegate->displayDidChange( notification );

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool DisplayController::saveConfigFile() const
{
    return saveCurrentConfiguration( _fileConfig );
}

/* **** **** **** **** **** **** **** */

bool DisplayController::loadConfigFile()
{
    if (! FileSystem::fileExists( _fileConfig ))
    return false;
    
    return true;
}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void DisplayController::updateContext()
{
    assert( calledFromThisThread() );
    
    _impl.update();
}
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void DisplayController::clearContext()
{
    assert( calledFromThisThread() );

    GXPath::clearRect( makeRect(makePointNULL(),  getCurrentMode().size), _clearColor );
}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool DisplayController::screenShot(const std::string &tofile) const
{
    return _impl.screenShot( tofile );
}

bool DisplayController::saveCurrentConfiguration( const std::string &file) const
{
    return getDisplayInformationsAsDatabase( getCurrentMode() ).saveToFile( file, '=');
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool DisplayController::loadConfigurationFile( const std::string &file)
{
    Database data;
    
    if ( data.parseFile( file, '=') )
        return setVideoModeTo( getDisplayInformationsFromDatabase( data ));
    
    else
        return false;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ Database DisplayController::getDisplayInformationsAsDatabase( const DisplayInformations &info)
{
   
    return
    {
        std::make_pair( "WIDTH"     , std::to_string ( info.size.width )  ),
        std::make_pair( "HEIGHT"    , std::to_string ( info.size.height ) ),
        std::make_pair( "FRAMERATE" , std::to_string ( info.framerate )   ),
        std::make_pair( "TYPE"      , std::to_string ( info.type )        ),
        std::make_pair( "NATIVE"    , std::to_string ( info.native )      ),
        std::make_pair( "RATIO"    , std::to_string  ( info.aspectRatio ) ),
    };
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ DisplayInformations DisplayController::getDisplayInformationsFromDatabase( Database &data)
{
    return  DisplayInformations::make( data.getValueForItemName("NATIVE")->getBool() ,
                                      (DisplayType)  data.getValueForItemName("TYPE")->getInt(),
                                      
                                       makeSize( data.getValueForItemName("WIDTH")->getInt() ,
                                                 data.getValueForItemName("HEIGHT")->getInt()
                                                ),
                                       data.getValueForItemName("FRAMERATE")->getInt(),
                                       data.getValueForItemName("RATIO")->getFloat(),
                                      (DisplayHDMIMode) data.getValueForItemName("HDMIMODE")->getInt()
                                      );
}


/*static*/ void DisplayController::dumpImplementation()
{
    Log::log("###################################");
    Log::log("GX Implementated functions");
    Log::log("Plateform : %s", ScopedPlateformConfig::getPlateformName().c_str() );
    /* **** **** **** **** **** **** **** **** */
#ifdef USE_GRAPHICS
    Log::log("USE_GRAPHICS Ok ");
#else
    Log::log("USE_GRAPHICS not installed ");
#endif
    /* **** **** **** **** **** **** **** **** */
    /* **** **** **** **** **** **** **** **** */
#ifdef USE_GRAPHICS_HELPERS
    Log::log("USE_GRAPHICS_HELPERS Ok ");
#else
    Log::log("USE_GRAPHICS_HELPERS not installed ");
#endif
    
    Log::log("Images support : ");
#ifdef HAVE_JPEG_LIB
    Log::log("\tJpeg Lib ");
#endif
#ifdef HAVE_PNG_LIB
    Log::log("\tPng Lib ");
#endif
    
#if  !defined( HAVE_JPEG_LIB ) && !defined( HAVE_PNG_LIB )
    Log::log("\tNo image support ");
#endif
    
    /* **** **** **** **** **** **** **** **** */
    
    Log::log("###################################");
}

#ifdef ENABLE_ELEMENT_SELECTOR
const Variant DisplayController::performSelectorWithArguments( const std::string &selector,const VariantList  &arguments,bool *validCall  )
{
    if( validCall)
    *validCall = true;
    
    if( selector == "dumpImplementation")
    {
        dumpImplementation();
        return Variant::null();
    }
    
    else if( selector == "setCallback")
    {
        _notifSelector = arguments.at(0).getString();
        _notifTarget   = Element::getElementById( arguments.at(1).getUInt64() );
        
        return Variant( _notifTarget != nullptr );
    }
    
    else if (selector == "setFrameRate")
    {
        setFrameRate(arguments.at(0).getFloat() );
        return Variant::null();
    }
    
    else if( selector == "powerOn")
    {
        powerOn();
        return Variant::null();
    }
    
    else if( selector == "powerOff")
    {
        powerOff();
        return Variant::null();
    }
    
    else if ( selector == "setBackgroundColor")
    {
        const GXColor col = makeColor( (uint8_t) arguments.at(0).getInt(),
                                      (uint8_t) arguments.at(1).getInt(),
                                      (uint8_t) arguments.at(2).getInt(),
                                      (uint8_t) arguments.at(3).getInt()
                                      );
        _clearColor = col;
        
        return Variant::null();
        
    }
    
    else if (selector == "clearScreen")
    {
        clearScreen();
        return Variant::null();
    }
    
    else if( selector == "setDisplayedElement")
    {
        GXElement* el = dynamic_cast<GXElement*>(Element::getElementById( arguments.at(0).getUInt64() ));
        
        if( el )
            setDisplayedElement( el );
        
        return Variant::null();
    }
    
    else if (selector == "getCurrentFrameRate")
    {
        return Variant( getCurrentFrameRate() );
    }
    
    else if( selector == "getCurrentMode")
    {
        /*
         bool        native;
         
         DisplayType type;
         GXSize      size;
         
         int         framerate;
         float       aspectRatio;*/
        
        
        
        const DisplayInformations &mode =getCurrentMode();
        
        return Variant( {
            Variant(mode.native ),
            Variant((int)mode.type ),
            Variant(mode.size.width ),
            Variant(mode.size.height ),
            Variant(mode.framerate ),
            Variant(mode.aspectRatio )
        } );
    }
    
    else if (selector == "getAvailableVideoMode")
    {
        
        VariantList v;
        
        for( const auto &mode : getAvailableVideoMode() )
        {
            v.push_back( Variant(
                         {
                            Variant(mode.native ),
                            Variant((int)mode.type ),
                            Variant(mode.size.width ),
                            Variant(mode.size.height ),
                            Variant(mode.framerate ),
                            Variant(mode.aspectRatio )
                          } )
                        );
        }
        return v ;
    }
    
    else if (selector == "setVideoModeTo")
    {

        return Variant::null();
    }
    
    if( validCall)
        *validCall = false;
    
    return Element::performSelectorWithArguments(selector, arguments, validCall);
}
#endif
