/*!
 *  \brief     DisplayController
 *  \details   Handles screen operations + holds the displayed element(s).
 *  \author    Manuel Deneu
 *  \version   0
 *  \date      2014
 *  \pre       .
 *  \bug       
 *  \warning   .
 *  \copyright GNU Public License.
 */

#ifndef __Broadway_test__DisplayController__
#define __Broadway_test__DisplayController__

#ifdef TARGET_RASPBERRY_PI
    #include <bcm_host.h>

#endif /* TARGET_RASPBERRY_PI */

#include <semaphore.h>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "../Internal/AbstractController.h"
#include "../Internal/AbstractDelegate.h"
#include "../Internal/Thread.h"

#include "../Scheduler/Event.h"

#include "../Plateforms/Plateform.h"

#include "../Data/Database.h"

#include "GXDefs.h"

#include "Impl/DisplayImpl.h"

#include "../GXDataType/GXColors.h"
#include "GXElement.h"
#include "GXPath.h"
#include "GXImage.h"

class EventDisplay : public Event
{
public:
    
    EventDisplay( DisplayNotification _notif , Element* sender) :
    Event ( Event_Display ,sender ),
    notification ( _notif )
    {}
    
    ~EventDisplay()
    {}
    
    DisplayNotification notification;
};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class DisplayControllerDelegate : public virtual AbstractDelegate
{
    friend class DisplayController;
    
public:
    virtual ~DisplayControllerDelegate() {}
    
protected:
    DisplayControllerDelegate() {}
    
    virtual void displayDidChange( DisplayNotification notification ) = 0;
    
};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class DisplayController : public  AbstractController,
                          public  Thread,
                          public  Element
{

    /* **** **** **** **** **** **** */
    // new methods, with DisplayImpl
    
    friend class DisplayImpl;
    friend class GXElement;
    
public:
    
    static DisplayController* getInstance()
    {
        if( s_instance==nullptr)
            s_instance = new DisplayController();
        
        return s_instance;
    }
    
    static void releaseInstance()
    {
        if (s_instance != nullptr )
        {
            delete s_instance;
            s_instance = nullptr;
        }
    }

    void setDelegate( DisplayControllerDelegate *delegate)
    {
        _delegate = delegate;
    }
    
    // if set to true, incomming osc messages will be passed as Dispatch Messages.
    void setAsyncCallback( bool async )
    {
        _asyncCallback = async;
    }
    
    bool start();
    bool stop();
    
    /* *** *** *** *** *** *** *** *** *** *** *** *** *** */
    
    /* Modes & infos */
    
    inline const std::vector< DisplayInformations > getAvailableVideoMode() const
    {
        return _impl.getAvailableVideoMode();
    }
    
    inline const DisplayInformations getCurrentMode() const
    {
        return _impl.getCurrentDisplayInformations();
    }
    
    inline bool setVideoModeTo( const DisplayInformations &mode , DisplaySettingsMatch matchMode = Resolution )
    {
        return _impl.setVideoModeTo( mode , matchMode );
    }
    
    inline int getAudioVideoLatency() const
    {
        return _impl.getAudioVideoLatency();
    }
    
    void setFrameRate( float rate)
    {
        _desiratedFrameRate = rate;
    }
    float getFrameRate() const
    {
        return _desiratedFrameRate;
    }
    
    float getCurrentFrameRate() const
    {
        return _frameRate;
    }
    
    /* *** *** *** *** *** *** *** *** *** *** *** *** *** */
    
    /* Load/save config */

    void setSaveOnFile( bool save)
    {
        _shouldSaveConfigOnFile = save;
    }
    
    bool saveActivated() const
    {
        return _shouldSaveConfigOnFile;
    }
    
    void setSaveFile( const std::string &file)
    {
        _fileConfig = file;
    }
    
    const std::string &getSaveFile() const
    {
        return _fileConfig;
    }
    
    /* Commands to send to the display */
    
    inline bool showInfosOnDisplay( bool show ) const
    {
        return _impl.showInfosOnDisplay( show );
    }
    
    bool powerOn()
    {
        if (_impl.sendTvPowerOn() )
            _displayIsOn = true;
        
        return _displayIsOn;
    }
    
    bool powerOff()
    {
        if ( _impl.sendTvPowerOff() )
            _displayIsOn = false;
        
        return _displayIsOn;
    }
    
    bool isDisplayOn() const
    {
        return _displayIsOn;
    }

    /* *** *** *** *** *** *** *** *** *** *** *** *** *** */
    
    /* Displayed Element's management */
    
    void setDisplayedElement( GXElement* element);
    
    void removeDisplayedElement();
    
    GXElement* getCurrentElement() const
    {
        return _currentElement;
    }
    
    void setCleanColor( GXColor color);
    void clearScreen();
    void update();
    
    /* *** *** *** *** *** *** *** *** *** *** *** *** *** */
    
    /* Utils */
    
    bool screenShot(const std::string &tofile) const;
    
    bool saveCurrentConfiguration( const std::string &file) const;
    bool loadConfigurationFile( const std::string &file);
    
    static Database getDisplayInformationsAsDatabase( const DisplayInformations &info);
    static DisplayInformations   getDisplayInformationsFromDatabase( Database &data);
    
    
    //! Will write on logging system infos about current implemented GXGraphics functions
    static void dumpImplementation();

    
#ifdef ENABLE_ELEMENT_SELECTOR
const Variant performSelectorWithArguments( const std::string &selector , const VariantList  &arguments, bool *validCall = nullptr ) ;
#endif
    
    
    /* *** *** *** *** *** *** *** *** *** *** *** *** *** */
    /* adds for OMX */
    
    bool isNativeDeinterlace() const
    {
        return true;
    }

private:
    
    void wakeUpThread()
    {
        ScopedLock l (_sync);
        _wakeUp.notify_all();
    }
    DisplayController();
    virtual ~DisplayController();
    void displayChangeNotification( DisplayNotification notification );
    
    bool saveConfigFile() const;
    bool loadConfigFile();
    

    DisplayImpl _impl;
    
    bool _displayIsOn;
    
    DisplayControllerDelegate *_delegate;
    bool _asyncCallback;
    
#ifdef ENABLE_ELEMENT_SELECTOR
    Element *_notifTarget;
    std::string _notifSelector;
#endif
    
    GXColor _clearColor;
    
    GXElement   *_currentElement;
    GXElement   *_nextElement;

    /* **** **** **** **** **** **** */

    void run();
    
    // called on GUI thread!!
    void clearContext();
    void updateContext();

    bool     m_shouldClearContext;
    bool     m_shouldForceUpdate;
    

    float _frameRate;          // image/s
    float _desiratedFrameRate; // images/s
    /* File config load/save */
    
    bool        _shouldSaveConfigOnFile;
    std::string _fileConfig;
    
    std::mutex    _sync;
    std::condition_variable _wakeUp;
    
    static DisplayController* s_instance;
};

#endif /* defined(__Broadway_test__DisplayController__) */
