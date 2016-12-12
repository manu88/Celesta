//
//  MainPlayer.h
//  
//
//  Created by Manuel Deneu on 26/07/14.
//
//

#ifndef ____MAINPLAYER__
#define ____MAINPLAYER__


#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <stdint.h>
#include <termios.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <getopt.h>
#include <string.h>
#include <vector>

#include <condition_variable>

//#define AV_NOWARN_DEPRECATED // ?



#include "OMXStreamInfo.h"

#include "utils/log.h"

#include "DllAvUtil.h"
#include "DllAvFormat.h"
#include "DllAvCodec.h"
#include "linux/RBP.h"

#include "OMXVideo.h"
#include "OMXAudioCodecOMX.h"
#include "utils/PCMRemap.h"
#include "OMXClock.h"
#include "OMXAudio.h"
#include "OMXReader.h"
#include "OMXPlayerVideo.h"
#include "OMXPlayerAudio.h"
#include "OMXPlayerSubtitles.h"


#include "OMXControl.h" // a suppr

#include "OMXThread.h"
#include "../../Internal/Thread.h"

#include "DllOMX.h"
#include "Srt.h"
#include "KeyConfig.h"
//
#include "Keyboard.h"

#include <string>
#include <utility>


typedef enum
{
    CONF_FLAGS_FORMAT_NONE,
    CONF_FLAGS_FORMAT_SBS,
    CONF_FLAGS_FORMAT_TB
    
} FORMAT_3D_T;






// when we repeatedly seek, rather than play continuously
#define TRICKPLAY(speed) (speed < 0 || speed > 4 * DVD_PLAYSPEED_NORMAL)

#define DISPLAY_TEXT(text, ms) if(m_osd) m_player_subtitles.DisplayText(text, ms)

#define DISPLAY_TEXT_SHORT(text) DISPLAY_TEXT(text, 1000)
#define DISPLAY_TEXT_LONG(text) DISPLAY_TEXT(text, 2000)


#define S(x) (int)(DVD_PLAYSPEED_NORMAL*(x))

typedef struct
{
    unsigned long millis;
    bool          fired;
    
} TCMark;

class GXVideo;

/* **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** */

class MainPlayer : public Thread
{
public:
    
    static int getInstancesCount()
    {
        return s_instances;
    }
    
    MainPlayer( GXVideo *parent);
    ~MainPlayer();
    
    void setClock(OMXClock *clockToUse)
    {
        m_av_clock = clockToUse;
    }


    void setVideoFileSource( const std::string &filePath )
    {
        m_filename = filePath;
    }
    
    std::string getVideoFileSource() const
    {
        return m_filename;
    }
    
    void setLooped( bool loop)
    {
        m_loop = loop;
    }
    
    bool isLooped() const
    {
        return m_loop;
    }
    
    void setBounds(int x , int y , int w , int h);
    void setLayerNum( int layer );
    
    void setAlpha( uint8_t alpha);
    
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
    
    void setOrientation( int orientation );
    
    int getOrientation() const noexcept
    {
        return _orientation;
    }
    
    
    void setAudioDeviceName (const std::string &device)
    {
//        deviceString = device;
    }
    
    int getStreamLength() // should go const ..
    {
        return ( int ) m_omx_reader.GetStreamLength() / 1000;
    }
    
    unsigned long getCurrentTC();
    
    void registerTC( unsigned long tc);
    void resetAllRegisteredTC();

    bool prepare();
    bool start(); // main entry point
    void pauseStream();
    void stop();
    void releasePlayer();
    
    void flipVisibilityTo( bool visible);
    
    void seekTo(double timeInS);

    bool isRunning() const
    {
        return m_isRunning;
    }
    
    bool isPaused() const
    {
        return m_Pause;
    }

    
    void SetSpeed(int iSpeed);
    
    // def method here
    void setSpeedAsync( int newSpeed );
    
    void FlushStreams(double pts);
    
    enum {  playspeed_slow_min = 0 ,
            playspeed_slow_max = 7 ,
            playspeed_rew_max = 8 ,
            playspeed_rew_min = 13 ,
            playspeed_normal = 14 ,
            playspeed_ff_min = 15 ,
            playspeed_ff_max = 19
         };
    
    int getEffectiveSpeed(int index)
    {   
        switch (index)
        {
            case 0:
                return S(0);
                
            case 1:
                return S(1/16.0);
                
            case 2:
                return S(1/8.0);

            case 3:
                return S(1/4.0);
                
            case 4:
                return S(1/2.0);
            
            case 5:
                return S(0.975);
                
            case 6:
                return S(1.0);
                
            case 7:
                return S(1.125);
                
            case 8:
                return S(-32.0);
                
            case 9:
                return S(-16.0);
                
            case 10:
                return S(-8.0);
                
            case 11:
                return S(-4);
                
            case 12:
                return S(-2);
                
            case 13:
                return S(-1);
                
            case 14:
                return S(1);
                
            case 15:
                return S(2.0);
                
            case 16:
                return S(4.0);
                
            case 17:
                return S(8.0);
                
            case 18:
                return S(16.0);
                
            case 19:
                return S(32.0);

            default:
                return 0;
        }
    }
    
    void setVolume( float vol)
    {
        if ( vol <0)
            vol = 0.f;
        m_volumeAudio = vol;
        
         m_player_audio.SetVolume( m_volumeAudio );
        
    }
    
    float getVolume() const
    {
        return m_volumeAudio;
    }
    


    /**/
    
    void signalSourceChange()
    {
        _sourceWillChange = true;
    }
    
    void signalSourceChanged()
    {
        _wakeUp.notify_all();
    }

    /**/

protected:
    
    void waitIfNeeded( std::unique_lock<std::mutex> &lock ,const double  startpts );
    /* Thread's starting point */
    void run();
    //void Process();
    
    void update();
    
    
private:
    bool runInt();
    
    bool openVideoPlayer();
    
    /* *** *** *** *** *** *** */
    // cleaned attributes
    
    OMXClock           *m_av_clock; // partageable avec d'autres instances
    OMXReader           m_omx_reader;
    OMXPlayerVideo      m_player_video;
    OMXPlayerAudio      m_player_audio;
    OMXPlayerSubtitles  m_player_subtitles;
    
    OMXAudioConfig    m_config_audio;
    OMXVideoConfig    m_config_video;
    
    /* *** */
    
    std::mutex              _sync;
    std::condition_variable _wakeUp;
    
    GXVideo         *_parent;
    vector< TCMark > _registeredTCNotif;
    std::string     m_filename;
    
    // flags
    bool    m_shouldPause;
    bool    m_isRunning;
    bool     _firstPacket;
    
    bool    _shouldChangeSpeed;
    int     _nextSpeed;
    
    bool    _shouldFlipVisibility;
    bool    _nextVisibilityState;
    
    bool    m_loop;
    
    bool    m_Pause;
    bool    m_stop;
    int     m_playspeed_current;
    
    bool    m_seek_flush;
    double  m_incr;
    
    volatile bool _sourceWillChange;
    
    float m_timeout;

    /* *** *** *** *** *** *** */
    
    bool        m_live;
    float       m_video_queue_size;
    float       m_video_fifo_size;
    float       m_latency;
    
    
    int               _orientation;
    FORMAT_3D_T       m_3d;
    
    float             m_volumeAudio;
    long              m_Amplification;
    
    bool              m_Deinterlace;
    bool              m_NoDeinterlace;

    OMX_IMAGEFILTERANAGLYPHTYPE m_anaglyph;
    
    bool              m_has_external_subtitles;
    std::string       m_external_subtitles_path;

    int               m_audio_index_use;

    OMXPacket         *m_omx_pkt;

    bool              m_no_hdmi_clock_sync;

    int               m_subtitle_index;
    
    bool              m_has_video;
    bool              m_has_audio;
    bool              m_has_subtitle;
    
    
    static int s_instances;

};

#endif /* defined(____MAINPLAYER__) */
