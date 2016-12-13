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
#ifndef __MediaCenter__ApplicationBase__
#define __MediaCenter__ApplicationBase__

#include "../Data/Database.h"
#include "../Scheduler/Dispatch.h"
#include "../Env_Variables.h"
#include "../Plateforms/Plateform.h"

#include "FileManager.h"


/*
class ApplicationEvent : public Event
{
    ApplicationEvent(
};
*/
class ApplicationBase : /*public Event ,*/ public Element
{
public:
    virtual ~ApplicationBase();

    /* *** *** */
    // Main mechanims
    
    enum { TimeToQuit = 100 }; //ms
    
    inline bool isRunning() const noexcept
    {
        return !_quitSent;
    }
    
    bool start();
    void restart();
    
    bool sendQuitSignal()
    {
        if( _quitSent )
            return false;
        
        _quitSent = true;
        Dispatch::getInstance()->create( TimeToQuit , 0, std::bind(&ApplicationBase::releaseApp , this ) , TimerIdentifiers::QuitSignal );

        return true;
    }
    
    void resetFlags()
    {
        _isInitialized = false;
        
        _shouldRestart = false;
        _quitSent = false;
    }
    
    /* *** *** */
    // Application configuration base
    
    //! Perform a deep reload of the Database. Everything will be erased prior to read!
    inline bool reloadDatabase()
    {
        return parseConfig();
    }
    
    //! return the database object.
    /* See @class Database. */
    const Database &getDatabase() const
    {
        return _appData;
    }
    
    const Variant* getDataValue( const std::string &name , const Variant &def ) const
    {
        if ( _appData.itemExists( name ) )
        {   
            return _appData.getValueForItemName( name );
        }
        else
            return &def;
        
    }
    
    inline bool dataValueExists( const std::string &name )
    {
        return _appData.itemExists( name );
    }
    
    inline bool setDataValueForName( const std::string &name , const Variant &value)
    {
        return _appData.setValueForItemName(name, value);
    }
    
    //! Change the application datafile. ReloadDatabase needs to be called in order to parse the new file
    void changeDataFile( const std::string &file )
    {
        _fileConfig = file;
    }
    
    //! get the app datafile's  full path
    const std::string &getDataFile() const noexcept
    {
        return _fileConfig;
    }
    
    /* *** *** */

    const std::string &getApplicationName() const noexcept
    {
        return getElementName();
    }
    
    /* *** *** */
    
    bool isInitialized() const
    {
        return _isInitialized;
    }

    /* *** *** */
    
    const Database &getConfig() const
    {
        return _appData;
    }
    
    bool saveConfig() const;
    bool reloadConfig();
    
    /* **** **** **** **** **** **** **** **** **** */
    
    Timecode isRunningFor() const
    {
        return Timecode::getCurrent() - _startTime;
    }
    
    const Timecode& isRunningSince() const
    {
        return _startTime;
    }
    
    /* **** **** **** **** **** **** **** **** **** */
    
    const FileManager* getFileManager() const
    {
        return _fileManager;
    }

    const std::string getTempDirectory() const
    {
        return FileSystem::correctPathIfNeeded( ScopedPlateformConfig::getTempDir()) + DefaultsValues::IdentifierPrefix + getApplicationName() +"/";
    }
    const std::string getWorkDirectory() const
    {
        return FileSystem::correctPathIfNeeded( ScopedPlateformConfig::getHomeDir() ) + DefaultsValues::IdentifierPrefix + getApplicationName() +"/";
    }
    
    bool writePidFile();
    bool deletePidFile();
    
#ifdef ENABLE_ELEMENT_SELECTOR
    const Variant performSelectorWithArguments( const std::string &selector , const VariantList  &arguments , bool *validCall = nullptr) ;
#endif
    
protected:
    
    ApplicationBase( const std::string &fileConfig);
    
    void setApplicationName( const std::string &name) noexcept
    {
        if( name != getApplicationName() )
            setElementName( name );
    }
    
    bool parseConfig();
    
    bool initializeApp();
    void releaseApp();
    
    virtual bool applicationWillStart();
    virtual void applicationDidStart();
    virtual void applicationWillStop();
    virtual void applicationDidStop();

private:
    
    
    
    FileManager *_fileManager;


    Database             _appData;
    std::string           _fileConfig;
    

    
    bool  _isInitialized;
    bool  _shouldRestart;
    
    bool _quitSent;

    
    Timecode _startTime;
    
    int _pidFileHandler;

    
};

#endif /* defined(__MediaCenter__ApplicationBase__) */
