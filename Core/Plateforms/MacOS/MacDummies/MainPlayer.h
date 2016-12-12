//
//  MainPlayer.h
//  Broadway_test
//
//  Created by Manuel Deneu on 05/10/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_test__MainPlayer__
#define __Broadway_test__MainPlayer__

#include <iostream>

#include "../../../GX/DisplayController.h"
#include "../../../Scheduler/Dispatch.h"

class OMXClock
{
public:
    OMXClock()
    {
        
    };
    
    ~OMXClock()
    {
        
    }
    
    long OMXMediaTime() const
    {
        return 0.0;
    }

    

    
};

class GXVideo;

class MainPlayer : public Thread
{
public:
    MainPlayer( GXVideo *parent) :
    Thread("MainPlayer"),
    _parent(parent),
    m_volumeAudio(1.f)
    {
        
    }
    ~MainPlayer()
    {
        
    }
    
    bool prepare()
    {
        return true;
    }
    
    void setClock(OMXClock *)
    {
        
    }
    
    void setVideoFileSource( const std::string &filePath )
    {
        m_file = filePath;
    }
    
    std::string getVideoFileSource() const
    {
        return m_file;
    }
    void setLooped(bool  )
    {
        
    }
    
    bool isLooped() const
    {
        return true;
    }
    
    void setBounds(int  , int , int  , int )
    {
        
    }
    
    void setLayerNum( int  )
    {
        
    }
    
    void setAlpha( uint8_t  )
    {
        
    }
    
    bool start()
    {
        return startThread();
    }
    
    void pauseStream()
    {

    }
    
    /*
     0(default) OMX_DISPLAY_ROT0
     90         OMX_DISPLAY_ROT90
     180        OMX_DISPLAY_ROT180
     270        OMX_DISPLAY_ROT270
     
     1          OMX_DISPLAY_MIRROR_ROT0
     91         OMX_DISPLAY_MIRROR_ROT90
     181        OMX_DISPLAY_MIRROR_ROT180
     271        OMX_DISPLAY_MIRROR_ROT270
     */
    
    void setOrientation( int orientation )
    {
        _orientation = orientation;
    }
    int getOrientation() const
    {
        return 0;
    }
    
    void stop()
    {
        stopThread();
    }
    
    void releasePlayer()
    {

    }
    
    void flipVisibilityTo( bool )
    {
        
    }
    
    bool isRunning() const
    {
        return false;
    }
    
    bool isPaused() const
    {
        return false;
    }
    
    void setAudioDeviceName (const std::string &)
    {
        
    }
    
    void setVolume( float vol)
    {
        if ( vol <0)
            vol = 0.f;
        m_volumeAudio = vol;
        
        
    }
    
    void SetSpeed(int )
    {
        
    }
    
    void setSpeedAsync( int  )
    {
        
    }
    
    float getVolume() const
    {
        return m_volumeAudio;
    }
    
    int getStreamLength() // should go const ..
    {
        return 100;
    }
    
    unsigned long getCurrentTC()
    {
        return 100;
    }
    
    void seekTo(double )
    {
        
    }
    
    void registerTC( unsigned long )
    {
        
    }
    
    void resetAllRegisteredTC()
    {
        
    }

    
    void signalSourceChange()
    {

    }
    
    void signalSourceChanged()
    {
        
    }
    
    
private:
    void run();
    
    GXVideo *_parent;
    float m_volumeAudio;
    bool _showInfosOnScreen;
    bool m_Pause;
    std::string m_file;
    
    int _orientation;
    
    
};

#endif /* defined(__Broadway_test__MainPlayer__) */
