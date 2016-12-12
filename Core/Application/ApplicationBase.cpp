//
//  ApplicationBase.cpp
//  MediaCenter
//
//  Created by Manuel Deneu on 22/03/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#include <fcntl.h>
#include <unistd.h>
#include "ApplicationBase.h"

#include "../Internal/Utils.h"

#include "../Log/Log.h"
#include "../Plateforms/Plateform.h"
#include "../Data/Date.h"
#include "../Env_Variables.h"

ApplicationBase::ApplicationBase( const std::string &fileConfig ) :
//Event( Event_AppBase , this ),

_fileManager( nullptr ),

_fileConfig     ( fileConfig ),
_isInitialized  ( false ),
_shouldRestart  ( false ),
_quitSent       ( false ),
_pidFileHandler ( -1    )
{
    setApplicationName( DefaultsValues::DefaultApplicationName );
    className = ClassNames::ApplicationBase;
    
    addTypeInfo( System );

}

ApplicationBase::~ApplicationBase()
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** */

bool ApplicationBase::parseConfig()
{
    _appData.clear();
    
    if ( !_appData.parseFile(_fileConfig, '=') )
        return false;
    
    setApplicationName( _appData.getValueForItemName( Parameters::ApplicationName )->getString() );
        
    return true;
}

bool ApplicationBase::saveConfig() const
{
    return _appData.saveToFile( _fileConfig , '=' );
}

bool ApplicationBase::reloadConfig()
{
    return parseConfig();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** */

bool ApplicationBase::applicationWillStart()
{
    return true;
}

void ApplicationBase::applicationDidStart()
{
    
}

void ApplicationBase::applicationWillStop()
{
    
}

void ApplicationBase::applicationDidStop()
{
}

/* **** **** **** **** **** **** **** **** **** **** **** **** */

bool ApplicationBase::writePidFile()
{
    if( _pidFileHandler != -1)
    {
        return false;
    }
    _pidFileHandler = open( (ScopedPlateformConfig::getTempDir() + getApplicationName() + ".pid").c_str(), O_RDWR|O_CREAT, 0600);
    
    if( _pidFileHandler != -1)
    {
        if (lockf( _pidFileHandler,F_TLOCK,0) != -1)
        {
            char str[10];

            sprintf(str,"%d\n",getpid());
            
            write(_pidFileHandler, str, strlen(str));
            return true;
        }
        else
        {
            printf("Unable to lock PID \n");
        }
    }

    return false;
}

bool ApplicationBase::deletePidFile()
{
    return close( _pidFileHandler ) == 0;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** */


bool ApplicationBase::initializeApp()
{
    if (!parseConfig() )
    {
        return false;
    }
    
    if(!FileSystem::folderExists(getTempDirectory() ))
    {
        if(!FileSystem::createDir( getTempDirectory() ))
            Log::log("Unable to create temp. folder '%s'" , getTempDirectory().c_str() );
    }
    if( !FileSystem::folderExists(getWorkDirectory() ))
    {
        if(!FileSystem::createDir( getWorkDirectory() ))
            Log::log("Unable to create work. folder '%s'" , getWorkDirectory().c_str() );
    }
    
    if( _fileManager)
        delete _fileManager;
    
    _fileManager = new FileManager( getWorkDirectory() );
    Dispatch::getInstance()->add( _fileManager );
    
    /**/
    
    _fileManager->createDirectory("sys");
    
    if( _fileManager->ressourceNamedExists("stats"))
    {        
        const std::string content  = _fileManager->readFile(_fileManager->getRessourceNamed("stats") );

        if( !_fileManager->ressourceNamedExists("CrashsLog"))
            _fileManager->createFile("sys/CrashsLog");
            
            _fileManager->appendToFile(_fileManager->getRessourceNamed("CrashsLog"), content+"\n");

    }

    
    _fileManager->createFile("sys/stats");
    
    _fileManager->writeFile( _fileManager->getRessourceNamed("stats") , Date::currentDateTime() );



    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** */

bool ApplicationBase::start()
{
    if( !writePidFile() )
        return false;
    do
    {
        Dispatch::getInstance()->setAsMainThread();
        
        _isInitialized = false;
        
        _shouldRestart = false;
        _quitSent = false;
        
        if ( !initializeApp() )
            return false;
        

        if ( !applicationWillStart() )
            return false;
        

        
        _startTime = Timecode::getCurrent();
        
        Dispatch::getInstance()->create( 10,
                                         0,
                                         std::bind(&ApplicationBase::applicationDidStart , this ) ,
                                         TimerIdentifiers::ApplicationDidStart
                                        );
        
        Dispatch::getInstance()->run();

        /* ... RUN HERE ...*/
        
        deletePidFile();
        
        applicationDidStop();
        
        if( _fileManager)
            delete _fileManager;
        
        _fileManager = nullptr;
        
        Dispatch::getInstance()->removeAllNotifications();
        
        Dispatch::dispose();
        
        FileSystem::removeDir( getTempDirectory() );

    } while (_shouldRestart == true );


    return true;
}

void ApplicationBase::restart()
{
    _shouldRestart = true;
    sendQuitSignal();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** */

void ApplicationBase::releaseApp()
{
    applicationWillStop();
    
    _fileManager->deleteFile("sys/stats");

    Dispatch::getInstance()->stop();
}

#ifdef ENABLE_ELEMENT_SELECTOR
const Variant ApplicationBase::performSelectorWithArguments( const std::string &selector , const VariantList  &arguments , bool *validCall)
{
    if ( validCall )
        *validCall = true;

    if( selector == "shutdown")
    {
        sendQuitSignal();
        system("sudo poweroff");
        
        return Variant::null();
    }
    
    else if( selector == "reboot")
    {
        sendQuitSignal();
        system("sudo reboot");
        return Variant::null();
    }
    else if ( selector == "suicide")
    {
        float * haha = nullptr;
        
        *haha = 42.0;

        return Variant::null(); // what's the point anyway... we're already dead
    }
    
    else if( selector == "isRunning")
    {
        return Variant( isRunning() );
    }
    
    /* **** */
    
    else if( selector == "getApplicationName")
    {
        return Variant( getApplicationName() );
    }
    
    else if( selector == "setApplicationName")
    {
        setApplicationName( arguments.at(0).getString() );
    }
    
    /* **** */
    // Data values
    
    else if (selector == "getDataValue")
        return *getDataValue( arguments.at(0).getString(), Variant::null() ) ;
    
    else if (selector == "setDataValue")
        return Variant ( setDataValueForName( arguments.at(0).getString(), arguments.at(1) ) );
    
    else if (selector == "saveConfig")
        return Variant ( saveConfig() );
    
    /* **** */
    // system utils
    
    else if (selector == "getTemperature")
        return Variant( System::getTemp() );
    
    else if( selector == "getGpuMem")
        return Variant( System::getGpuMem());
    
    else if( selector== "getCpu")
        return Variant( std::get<0>(System::getCpuAndMemPercent()) );

    else if( selector== "getMem")
        return Variant( std::get<1>(System::getCpuAndMemPercent()) );
    
    else if ( selector == "getStartTime" )
    {
        return isRunningSince().getVariant();
    }
    else if (selector =="getTimeSinceStart")
        return Variant(  isRunningFor().getInMs() /1000);
    
    else if (selector =="getPlateformName")
        return Variant( ScopedPlateformConfig::getPlateformName() );
    
    /* **** */
    // logging system
    else if( selector=="log")
    {
        Log::log(arguments.at(0).getString().c_str() );
        return Variant::null();
    }
    else if( selector=="addLocalLogger")
    {
        Log::addLocalLogger();
        return Variant::null();
    }
    
    else if( selector == "addFileLogger")
    {
        Log::addFileLogger( getWorkDirectory()+"Log.txt" );
        return Variant::null();
    }
    
    else if( selector == "getTempDirectory")
    {
        return Variant( getTempDirectory() );
    }
    
    else if( selector == "getApplicationFolder")
    {
        return Variant( getWorkDirectory() );
    }
    /* **** */
    if ( validCall )
        *validCall = false;
    
    return Element::performSelectorWithArguments(selector, arguments, validCall );
}
#endif



