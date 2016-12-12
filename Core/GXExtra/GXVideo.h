//
//  GXVideo.h
//  
//
//  Created by Manuel Deneu on 13/08/14.
//
//

#ifndef ____GXVideo__
#define ____GXVideo__

#include <iostream>
#include "../GX/GXElement.h"

#include "../GX/DisplayController.h"

#include "../Scheduler/Timecode.h"
#include "../Scheduler/Event.h"

#include "../Internal/FileSystem.h"

/* **** **** **** **** **** **** **** **** **** **** **** */
// Actual implementation.

// On raspberry Pi, uses OMXPlayer
// see https://github.com/popcornmix/omxplayer

#ifdef TARGET_RASPBERRY_PI
    #include "OMX/MainPlayer.h"


// Dummy impl on Mac OS !
#else
    #include "../Plateforms/MacOS/MacDummies/MainPlayer.h"


#endif

/* **** **** **** **** */

//! Audio output representation
/*
    Todo : create audioGraph processor, add alsa support for external sound cards, ...
 */
typedef enum
{
    AUDIO_NONE    = -1,
    AUDIO_HDMI    = 0,
    AUDIO_LOCAL   = 1,
    AUDIO_BOTH    = 2
    
} AUDIO_OUTPUT;

/* **** **** **** **** */

//! Notifications sent to SchedulerDelegate instance ( optionnal ).
/*
    These notifications are async and sent when possible :
    Its garranted that they will be fired on time, or possibly after a short delay, but _NEVER_ before the event! 
 */
typedef enum 
{
    VideoLoaded     = 0, // Sent after the player has been loaded and ready to roll.
    VideoWillStart  = 1, // Sent when player enters playing loop.
    VideoDidStart   = 2, // Sent when demuxer sent first frames.
    VideoPaused     = 3, // Sent when pause command performed.
    VideoDidResume  = 4, // Sent when resume command performed.
    VideoWillEnd    = 5, // Sent when video demuxer is end-of-stream.
    VideoDidEnd     = 6, // Sent when player did stop, i.e after flushing last frames.
    VideoDidReachTC = 7, // Notif fired when a registered TC is reached.
    
} GXVideoNotification;

/* **** **** **** **** */

class VideoEvent : public Event
{
    friend class GXVideo;
public:
    
    GXVideoNotification notification;
    
private:
    VideoEvent( GXVideo *video , GXVideoNotification notif );
    
    ~VideoEvent()
    {}

};

/* **** **** **** **** */

class GXVideo : public GXElement
{
public:
 
    typedef enum
    {
        Default     = 0,    // OMX_DISPLAY_ROT0
        Rotation90  = 90,   // OMX_DISPLAY_ROT90
        Rotation180 = 180,  // OMX_DISPLAY_ROT180
        Rotation270 = 270,  // OMX_DISPLAY_ROT270
        /*
        1          OMX_DISPLAY_MIRROR_ROT0
        91         OMX_DISPLAY_MIRROR_ROT90
        181        OMX_DISPLAY_MIRROR_ROT180
        271        OMX_DISPLAY_MIRROR_ROT270
         */
    } VideoOrientation;
    
    
    friend class MainPlayer;
    GXVideo();
    ~GXVideo();
    

    /**/
    
    inline void setAlpha( uint8_t alpha) 
    {
        _player.setAlpha( alpha);
        GXElement::setAlpha( alpha );
    }

    /* Player options */
    // unify with changeFileTo
    bool setFileSource( const std::string &filePath )
    {
        if( filePath != getFileSource() )
        {
            if (!FileSystem::fileExists( filePath ))
                return false;
        
            _player.setVideoFileSource( filePath );
            
            updateValueForKey( GXElementAttributes::Ressource, Variant(filePath ));

        }
        
        return true;
        
    }

    std::string getFileSource() const
    {
        return _player.getVideoFileSource();
    }

    void setLooped(bool loop )
    {
        _player.setLooped( loop );
    }
    
    bool isLooped() const
    {
        return _player.isLooped();
    }
    
    bool prepare()
    {

        m_isPrepared = _player.prepare();
        getVideoLength();
        
        return m_isPrepared;
    }

    void releasePlayer()
    {
        _player.releasePlayer();
        m_isPrepared = false;
    }
    
    void setOrientation( VideoOrientation orientation )
    {
        _player.setOrientation( orientation );
    }
    
    VideoOrientation getOrientation() const
    {
        return ( VideoOrientation ) _player.getOrientation();
    }
    
    /* **** **** **** **** **** **** **** **** */
    // telecommand
    
    inline bool start()
    {
        return _player.start();
    }
    
    inline bool pause()
    {
        if ( !_player.isPaused())
        {
            _player.pauseStream();
            return true;
        }
        
        return false;
    }
    
    inline bool resume()
    {
        if(!start())
        {
            if (_player.isPaused() )
            {
                _player.pauseStream();
                return true;
            }
        }
        return false;
    }

    inline void stop()
    {
        _player.stop();
        m_isPrepared = false;
    }
    
    void seekToTC( Timecode tc );
    
    void  setSpeed( float speed );
    float getSpeed() const;
    
    /* **** **** **** **** **** **** **** **** */
    // accessors
    
    bool isStarted() const
    {
        return _player.isRunning();
    }
    
    bool isPaused() const
    {
        return _player.isPaused();
    }

    Timecode getVideoLength();
    
    long getMediaTime()// const
    {
        return m_clock.OMXMediaTime();
    }
    
    Timecode getCurrentTC()
    {
        return Timecode(0,0,0, (unsigned int) _player.getCurrentTC() );
    }
    
    Timecode getRemainingTC()
    {
        return (getVideoLength() - getCurrentTC());
    }

    void registerTCNotification( const Timecode &tc);
    void resetAllTCNotification();
    

    // audio
    void setAudioOutput( const AUDIO_OUTPUT &output);

    void setVolume( float vol)
    {
        _player.setVolume( vol );
    }
    
    float getVolume() const
    {
        return _player.getVolume();
    }
    
    /**/
    
    GXVideoNotification getNotification() const
    {
        return _notif;
    }

    bool changeFileTo( const std::string &file );
    
#ifdef ENABLE_ELEMENT_SELECTOR
    
    void setNotificationSelector( const std::string selector, Element* target)
    {
        _notifSelector = selector;
        _notifTarget   = target;
    }
    const Variant performSelectorWithArguments( const std::string &selector , const VariantList  &arguments , bool *validCall = nullptr) ;
#endif

    /* Reimpl from GXElement */
    
    void setBounds( const GXRect &bounds );
    void setLayer(int layer);
    
protected:
    
    //! Tell the demux. to stop running so changes can be performed
    /*
        streams will be flushed.
     */
    void willChange()
    {
        _player.signalSourceChange();
    }

    //! Tell the demux. to start running again after a reinitialization.
    void didChange()
    {
        _player.signalSourceChanged();
    }
    
    void paint( const GXRect &rect , GXAnimation* anim );
    void prepareRessources();
    void deleteRessources();
    void changed();
    
    /* Called by mainPlayer */
    
    void sig_ready();
    void sig_willStart();
    void sig_didStart();
    
    void sig_didPause();
    void sig_didResume();
    
    void sig_willEnd();
    void sig_didEnd();
    
    void sig_didReachTC( unsigned long millis);
    
    GXVideoNotification _notif;
    
private:
    
    void sendNotification( GXVideoNotification notif );
    
#ifdef USE_JSON_PARSER
    bool addJSONinfos( JSON::Node &node) const;
#endif
    
    Element     *_notifTarget;
    std::string  _notifSelector;
    
    AUDIO_OUTPUT m_audioOutput;

    MainPlayer   _player; // real implementation
    
    bool         m_isPrepared;
    
    Timecode     m_length;
    OMXClock     m_clock;
    

    
};


#endif /* defined(____GXVideo__) */
