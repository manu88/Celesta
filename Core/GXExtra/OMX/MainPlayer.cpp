//
//  Broadway.cpp
//  
//
//  Created by Manuel Deneu on 26/07/14.
//
//

extern "C"
{
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
};

#include "MainPlayer.h"
#include "utils/Strprintf.h"

#include "../GXVideo.h"

#include "../../Internal/FileSystem.h"
#include "../../Internal/Utils.h"

#include "../../GX/DisplayController.h"


/*static*/ int MainPlayer::s_instances = 0;

MainPlayer::MainPlayer( GXVideo *parent) :
    Thread("MainPlayer"),
    m_av_clock            ( nullptr ),
    _parent               ( parent ),

    m_shouldPause         ( false ),
    m_isRunning           ( false ),
    _firstPacket          ( true  ),

    _shouldChangeSpeed    ( false ),
    _nextSpeed            ( 0     ),

    _shouldFlipVisibility ( false ),
    _nextVisibilityState  ( false ),

    m_loop                ( false   ),
    m_Pause               ( false   ),
    m_stop                ( false   ),
    m_playspeed_current   ( playspeed_normal ),

    m_seek_flush          ( false ),
    m_incr                ( 0     ),

    _sourceWillChange ( false ),

    m_timeout             ( 10.0f),

    m_live                ( false ),
    m_video_queue_size    ( 0.0f  ),
    m_video_fifo_size     ( 0.0f  ),
    m_latency             ( 0.0f  ),

    _orientation          ( -1    ), // unset

    m_3d                  ( CONF_FLAGS_FORMAT_NONE ),

    m_volumeAudio         ( 1.f   ),

    m_Amplification       ( 0     ),
    m_Deinterlace         ( false ),
    m_NoDeinterlace       ( false ),
    m_anaglyph            ( OMX_ImageFilterAnaglyphNone ),

    m_has_external_subtitles ( false ),

    m_audio_index_use     ( 0       ),

    m_omx_pkt             ( nullptr ),

    m_no_hdmi_clock_sync  ( false   ),

    m_subtitle_index      ( -1      ),

    m_has_video           ( false   ),
    m_has_audio           ( false   ),
    m_has_subtitle        ( false   )
{
    s_instances++;
}

/* **** **** **** **** **** **** **** **** **** **** **** */

MainPlayer::~MainPlayer()
{
    _wakeUp.notify_all();
    _registeredTCNotif.clear();
    
    s_instances--;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

bool MainPlayer::start()
{

    if (!m_isRunning)
    {
        m_isRunning = true;

        return startThread();
    }


    return false;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

void MainPlayer::stop()
{
    
    m_stop = true;
    m_isRunning = false;
    m_incr = 0;
    
    stopThread();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

void MainPlayer::seekTo(double timeinS)
{

    double oldPos = m_av_clock->OMXMediaTime()*1e-6;
    m_incr = timeinS - oldPos;
    
    resetAllRegisteredTC();
}

void MainPlayer::setBounds(int x , int y , int w , int h)
{
    m_config_video.dst_rect.x1 = x;
    m_config_video.dst_rect.y1 = y;
    
    m_config_video.dst_rect.x2 = x + w;
    m_config_video.dst_rect.y2 = y + h;
    

    if(m_isRunning)
        m_player_video.SetVideoRect(m_config_video.src_rect, m_config_video.dst_rect);



}

void MainPlayer::setLayerNum( int layer )
{
    m_config_video.layer = layer;
}

void MainPlayer::setOrientation( int orientation )
{
    if( _orientation != orientation )
        _orientation = orientation;
}

void MainPlayer::setAlpha( uint8_t alpha)
{
    m_config_video.alpha = alpha;
    m_player_video.SetAlpha( alpha );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

void MainPlayer::SetSpeed( int iSpeed )
{
//    Lock();
//    iSpeed = getEffectiveSpeed( iSpeed );
    if( !m_av_clock )
        return;
    
    m_omx_reader.SetSpeed(iSpeed);
    
    // flush when in trickplay mode

    if (TRICKPLAY(iSpeed) || TRICKPLAY(m_av_clock->OMXPlaySpeed()))
    {
        FlushStreams(DVD_NOPTS_VALUE);
    }

    
    m_av_clock->OMXSetSpeed(iSpeed  );
    m_seek_flush = true;
    
//    UnLock();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

void MainPlayer::FlushStreams(double pts)
{
    m_av_clock->OMXStop();
    m_av_clock->OMXPause();
    
    if(m_has_video)
        m_player_video.Flush();
    
    if(m_has_audio)
        m_player_audio.Flush();
    
    if(pts != DVD_NOPTS_VALUE)
        m_av_clock->OMXMediaTime(0.0);

    if(m_omx_pkt)
    {
        m_omx_reader.FreePacket(m_omx_pkt);
        m_omx_pkt = NULL;
    }
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

bool MainPlayer::prepare()
{
    m_config_video.hdmi_clock_sync = true;
    
    bool refresh = false ;


    float threshold      = -1.0f; // amount of audio/video required to come out of buffering

    float fps            =  0.0f; // unset
    
    
    enum PCMLayout m_layout = PCM_LAYOUT_2_0;
    
    std::string mode;

    
    bool filename_is_URL = FileSystem::isURL( m_filename );
    
    if( !filename_is_URL && !FileSystem::isPipe( m_filename ) && !FileSystem::fileExists( m_filename ))
    {
        printf("\n FAILED file source not found \n ");
        return false;
    }

    if(m_has_external_subtitles && !FileSystem::fileExists(m_external_subtitles_path))
    {
        printf("\n FAILED file subtitles italic not found \n ");
        return false;
    }
    
    if(!m_has_external_subtitles && !filename_is_URL)
    {
        auto subtitles_path = m_filename.substr(0, m_filename.find_last_of(".")) +
        ".srt";
        
        if(FileSystem::fileExists(subtitles_path))
        {
            m_external_subtitles_path = subtitles_path;
            m_has_external_subtitles = true;
        }
    }
    
    bool m_audio_extension = false;
    
    const CStdString m_musicExtensions = ".nsv|.m4a|.flac|.aac|.strm|.pls|.rm|.rma|.mpa|.wav|.wma|.ogg|.mp3|.mp2|.m3u|.mod|.amf|.669|.dmf|.dsm|.far|.gdm|"
    ".imf|.it|.m15|.med|.okt|.s3m|.stm|.sfx|.ult|.uni|.xm|.sid|.ac3|.dts|.cue|.aif|.aiff|.wpl|.ape|.mac|.mpc|.mp+|.mpp|.shn|.zip|.rar|"
    ".wv|.nsf|.spc|.gym|.adx|.dsp|.adp|.ymf|.ast|.afc|.hps|.xsp|.xwav|.waa|.wvs|.wam|.gcm|.idsp|.mpdsp|.mss|.spt|.rsd|.mid|.kar|.sap|"
    ".cmc|.cmr|.dmc|.mpt|.mpd|.rmt|.tmc|.tm8|.tm2|.oga|.url|.pxml|.tta|.rss|.cm3|.cms|.dlt|.brstm|.wtv|.mka";
    
    if ( m_filename.find_last_of(".") != string::npos)
    {
        CStdString extension = m_filename.substr( m_filename.find_last_of("."));
        
        if (!extension.IsEmpty() && m_musicExtensions.Find(extension.ToLower()) != -1)
            m_audio_extension = true;
        
    }
    
    else
    {
        CLog::SetLogLevel(LOG_LEVEL_NONE);
    }
    
    const int gpu_mem = System::getGpuMem();
    const int min_gpu_mem = 64;
    
    if ( gpu_mem > 0 && gpu_mem < min_gpu_mem )
        printf("Only %dM of gpu_mem is configured. Try running \"sudo raspi-config\" and ensure that \"memory_split\" has a value of %d or greater\n", gpu_mem, min_gpu_mem);
    

    if(!m_omx_reader.Open( m_filename.c_str(),
                           false /*dump_format*/,
                           m_config_audio.is_live,
                           m_timeout,
                           "" /*cooke*/, "" /*user agent*/
                          )
       )
    {
        printf("\n FAILED m_omx_reader.Open \n ");
        return false;
    }
    
    m_has_video     = m_omx_reader.VideoStreamCount();
    m_has_audio     = m_audio_index_use < 0 ? false : m_omx_reader.AudioStreamCount();
    m_has_subtitle  = false; //m_has_external_subtitles || m_omx_reader.SubtitleStreamCount();
    m_loop          = m_loop && m_omx_reader.CanSeek();
    
    if (m_audio_extension)
    {
        CLog::Log(LOGWARNING, "%s - Ignoring video in audio filetype:%s", __FUNCTION__,  m_filename.c_str());
        m_has_video = false;
    }
    
    if(  m_filename.find("3DSBS") != string::npos || m_filename.find("HSBS") != string::npos)
        m_3d = CONF_FLAGS_FORMAT_SBS;
    
    else if( m_filename.find("3DTAB") != string::npos || m_filename.find("HTAB") != string::npos)
        m_3d = CONF_FLAGS_FORMAT_TB;
    
    // 3d modes don't work without switch hdmi mode
    if (   m_3d != CONF_FLAGS_FORMAT_NONE
        || DisplayController::getInstance()->isNativeDeinterlace()
       )
        refresh = true;
    
    // you really don't want want to match refresh rate without hdmi clock sync
    if (    ( refresh || DisplayController::getInstance()->isNativeDeinterlace() )
        &&  !m_no_hdmi_clock_sync
       )
    {
        m_config_video.hdmi_clock_sync = true;
        //m_hdmi_clock_sync = true;
    }

    if( m_av_clock == nullptr )
    {
        printf("No clock ! \n");
        return false;
    }
    if(!m_av_clock->OMXInitialize())
    {
        printf("\n FAILED : m_av_clock->OMXInitialize()\n ");
        return false;
    }
    if(m_config_video.hdmi_clock_sync && !m_av_clock->HDMIClockSync())
    {
        printf("\n FAILED m_hdmi_clock_sync && !m_av_clock->HDMIClockSync() \n ");
        return false;
    }

    m_av_clock->OMXStateIdle();
    m_av_clock->OMXStop();
    m_av_clock->OMXPause();
    
    m_omx_reader.GetHints(OMXSTREAM_AUDIO, m_config_audio.hints);
    m_omx_reader.GetHints(OMXSTREAM_VIDEO, m_config_video.hints);
    
    // le fps a été passé en arg
    if ( fps > 0.0f )
        m_config_video.hints.fpsrate = fps * DVD_TIME_BASE, m_config_video.hints.fpsscale = DVD_TIME_BASE;
    
    if(m_audio_index_use > 0)
        m_omx_reader.SetActiveStream(OMXSTREAM_AUDIO, m_audio_index_use-1);
    
    if ( _orientation >= 0)
        m_config_video.hints.orientation = _orientation;
    
    if(    m_has_video && !openVideoPlayer() )
    {
        printf("\n FAILED : openVideoPlayer()\n ");
        return false;
    }
    
    m_player_video.SetVideoRect(m_config_video.src_rect, m_config_video.dst_rect);

    m_omx_reader.GetHints(OMXSTREAM_AUDIO, m_config_audio.hints);

    m_config_audio.device = "omx:both";//"omx:hdmi";

    if(m_has_audio && !m_player_audio.Open(m_av_clock, m_config_audio, &m_omx_reader))
    {
        printf("\n FAILED : m_player_audio.Open()\n ");
        return false;
    }
    
    if( m_has_audio )
    {
        m_player_audio.SetVolume( m_volumeAudio /*pow(10, m_Volume / 2000.0)*/);
        
        if ( m_Amplification )
            m_player_audio.SetDynamicRangeCompression(m_Amplification);
    }
    
    if ( threshold < 0.0f)
        threshold = m_live ? 0.7f : 0.2f;
    
    //    PrintSubtitleInfo();
    
    
    _parent->sig_ready();
    
    _firstPacket = true;

    return true;
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

void MainPlayer::pauseStream()
{
    if ( m_isRunning)
        m_shouldPause = true;

}

void MainPlayer::setSpeedAsync( int newSpeed )
{
    if( _nextSpeed == 0 )
    {
        _nextSpeed = newSpeed;
        _shouldChangeSpeed = true;
    }
}

void MainPlayer::flipVisibilityTo( bool visible)
{
    /*
    if ( m_has_video != visible )
    {*/
    _shouldFlipVisibility = true;
    _nextVisibilityState  = visible;
    
    //}
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

unsigned long MainPlayer::getCurrentTC()
{
    return (m_av_clock->OMXMediaTime()*1e-3);
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

void MainPlayer::registerTC( unsigned long tc)
{
    _registeredTCNotif.push_back( { tc , false} );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

void MainPlayer::resetAllRegisteredTC()
{

    for ( TCMark &t : _registeredTCNotif )
    {
        t.fired = false;
    }
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

void MainPlayer::update()
{
       
    
    //getEffectiveSpeed
    double now = m_av_clock->GetAbsoluteClock();
    //m_player_subtitles.DisplayText( strprintf("%ld", now) , 10 /*time to display in MS*/);
    
    /* **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
    if( _shouldChangeSpeed )
    {
        if( _nextSpeed != 0 )//&& _nextSpeed != m_pl)
        {
            SetSpeed( _nextSpeed );
        }
        _nextSpeed = 0;
        _shouldChangeSpeed = false;
    }
    if ( m_shouldPause )
    {
        m_Pause = !m_Pause;
        
        if (m_av_clock->OMXPlaySpeed() != DVD_PLAYSPEED_NORMAL && m_av_clock->OMXPlaySpeed() != DVD_PLAYSPEED_PAUSE)
        {
            /*
            m_playspeed_current = playspeed_ff_max;
            SetSpeed( getEffectiveSpeed( m_playspeed_current ) );
            */
            m_seek_flush = true; //
        }
        
        /* Pause requested */
        if( m_Pause )
        {
            /*
            if(m_has_subtitle)
                m_player_subtitles.Pause();
            */
            auto t = (unsigned) (m_av_clock->OMXMediaTime()*1e-6);
            auto dur = m_omx_reader.GetStreamLength() / 1000;
            
            _parent->sig_didPause();
            
        }
        /* Resume requested */
        else
        {
            /*
            if( m_has_subtitle )
                m_player_subtitles.Resume();
            */
            auto t = (unsigned) (m_av_clock->OMXMediaTime()*1e-6);
            auto dur = m_omx_reader.GetStreamLength() / 1000;


            //_parent->sig_didResume();
        }
        
        m_shouldPause = false;
    }
    
    /* **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
    
    if (_shouldFlipVisibility)
    {

        if (m_has_video && (_nextVisibilityState == false )) // hide
        {
            m_has_video = false;
            m_player_video.Close();
        }
        else  if ( _nextVisibilityState == true )// show
        {
            m_has_video = true;

            
            m_seek_flush = true;
        }

        _shouldFlipVisibility = false;
    }
    
}


bool MainPlayer::openVideoPlayer()
{
    return m_player_video.Open(m_av_clock, m_config_video);
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/
/*
    Main loop
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

void MainPlayer::waitIfNeeded( std::unique_lock<std::mutex> &lock ,const double  startpts )
{
    //FlushStreams( startpts );

    _sourceWillChange = false;
    _wakeUp.wait( lock );
    

    
    m_av_clock->OMXReset(m_has_video, m_has_audio);
    m_av_clock->OMXStateExecute();
}

bool MainPlayer::runInt()
{

    
    Thread::sleepForMs( 1000 );

    double loop_from         = 0 ;
    double last_check_time   = 0.0 ;
    
    double startpts          = 0;
    bool   sentStarted       = false;

    double last_seek_pos     = 0;
    bool   idle              = false;
    
    bool   send_eos          = false;
    bool   end_sent          = false;
    bool   packet_after_seek = false;
    
    _firstPacket = true;
    
    m_stop = false;
    
    m_av_clock->OMXReset(m_has_video, m_has_audio);
    m_av_clock->OMXStateExecute();

    
    m_playspeed_current = playspeed_ff_max/* playspeed_normal*/;
    
    m_isRunning = true;
    
    _parent->sig_willStart();
    

    while( !threadShouldStop() && !m_stop )
    {
        std::unique_lock<std::mutex> lock( _sync );
        
        /**/
        
        if (_sourceWillChange)
        {
            waitIfNeeded( lock , startpts );
            
            end_sent = false;
        }
        /**/
        
        double now = m_av_clock->GetAbsoluteClock();
        
        /* **** **** **** **** **** **** **** **** */
        /* TC NOTIF */

        for ( TCMark &t : _registeredTCNotif )
        {
            if ( ( t.fired == false )  && ( getCurrentTC() >= t.millis)  )
            {
                t.fired = true;
                _parent->sig_didReachTC( t.millis );
            }
        }
        
        /* END OF TC NOTIF */
        /* **** **** **** **** **** **** **** **** */
        
        bool shouldUpdate = false;
            
        if ( last_check_time == 0.0 || last_check_time + DVD_MSEC_TO_TIME(20) <= now )
        {
            shouldUpdate = true;
            last_check_time = now;
        }

        // a priori que sur de la source live
        if ( idle )
        {
            usleep(10000);
            continue;
        }
        
        // seek flush
        if( m_seek_flush || m_incr != 0)
        {
            double seek_pos     = 0;
            double pts          = 0;
            /*
            if(m_has_subtitle)
                m_player_subtitles.Pause();
            */
            pts = m_av_clock->OMXMediaTime();
            
            seek_pos = (pts ? pts / DVD_TIME_BASE : last_seek_pos) + m_incr;
            last_seek_pos = seek_pos;
            
            seek_pos *= 1000.0;
            
            m_incr = 0;
            
            if( m_omx_reader.SeekTime( (int) seek_pos, m_incr < 0.0f, &startpts ) )
            {
                unsigned t = (unsigned)(startpts*1e-6);
                auto dur = m_omx_reader.GetStreamLength() / 1000;
                
                FlushStreams( startpts );
             }
            
            
            m_player_video.Close();
            
            sentStarted = false;
            
            if(    m_has_video && !openVideoPlayer() )
                    return false;
            
            m_av_clock->OMXPause();
            
            packet_after_seek = false;
            
            m_seek_flush = false;
            
        } // end of seek flush
        
        /* **** **** **** **** **** **** **** **** **** **** **** */
        
        
        else if( packet_after_seek && TRICKPLAY(m_av_clock->OMXPlaySpeed()) )
        {

            double seek_pos     = 0;
            double pts          = 0;
            
            pts = m_av_clock->OMXMediaTime();
            seek_pos = (pts / DVD_TIME_BASE);
            
            seek_pos *= 1000.0;
            
            if(m_omx_reader.SeekTime((int)seek_pos, m_av_clock->OMXPlaySpeed() < 0, &startpts))
                ;
            unsigned t = (unsigned)(pts*1e-6);

            packet_after_seek = false;
        }
        
        
        /* player got in an error state */
        if(m_player_audio.Error())
        {
            printf("audio player error. emergency exit!!!\n");
            return false;
        }
        
        if ( shouldUpdate )
        {
            
            update();
            /* when the video/audio fifos are low, we pause clock, when high we resume */
            
            double stamp     = m_av_clock->OMXMediaTime();
            double audio_pts = m_player_audio.GetCurrentPTS();
            double video_pts = m_player_video.GetCurrentPTS();
            
            // utile ??
            if (0 && m_av_clock->OMXIsPaused())
            {
                double old_stamp = stamp;
                if (audio_pts != DVD_NOPTS_VALUE && (stamp == 0 || audio_pts < stamp))
                    stamp = audio_pts;
                
                if (video_pts != DVD_NOPTS_VALUE && (stamp == 0 || video_pts < stamp))
                    stamp = video_pts;
                
                if (old_stamp != stamp)
                {
                    m_av_clock->OMXMediaTime(stamp);
                    stamp = m_av_clock->OMXMediaTime();
                }
            }
            
            float audio_fifo = audio_pts == DVD_NOPTS_VALUE ? 0.0f : audio_pts / DVD_TIME_BASE - stamp * 1e-6;
            float video_fifo = video_pts == DVD_NOPTS_VALUE ? 0.0f : video_pts / DVD_TIME_BASE - stamp * 1e-6;
            float threshold = std::min(0.1f, (float)m_player_audio.GetCacheTotal() * 0.1f);
            
            bool audio_fifo_low  = false;
            bool video_fifo_low  = false;
            bool audio_fifo_high = false;
            bool video_fifo_high = false;
            
            
            
            if (audio_pts != DVD_NOPTS_VALUE)
            {
                audio_fifo_low  =  m_has_audio && audio_fifo < threshold;
                audio_fifo_high = !m_has_audio || (audio_pts != DVD_NOPTS_VALUE && audio_fifo > threshold);
            }
            if (video_pts != DVD_NOPTS_VALUE)
            {
                video_fifo_low = m_has_video && video_fifo < threshold;
                video_fifo_high = !m_has_video || (video_pts != DVD_NOPTS_VALUE && video_fifo > threshold);
            }
            
            CLog::Log(LOGDEBUG, "Normal M:%.0f (A:%.0f V:%.0f) P:%d A:%.2f V:%.2f/T:%.2f (%d,%d,%d,%d) A:%d%% V:%d%% (%.2f,%.2f)\n",
                                stamp, audio_pts,
                                video_pts,
                                m_av_clock->OMXIsPaused(),
                                audio_pts == DVD_NOPTS_VALUE ? 0.0:audio_fifo,
                                video_pts == DVD_NOPTS_VALUE ? 0.0:video_fifo,
                                threshold, audio_fifo_low,
                                video_fifo_low,
                                audio_fifo_high,
                                video_fifo_high,
                                m_player_audio.GetLevel(),
                                m_player_video.GetLevel(),
                                m_player_audio.GetDelay(),
                                (float)m_player_audio.GetCacheTotal()
                      );
            

            if(    !m_Pause
                      && (    m_omx_reader.IsEof()
                           || m_omx_pkt
                           || TRICKPLAY( m_av_clock->OMXPlaySpeed() )
                           || ( audio_fifo_high && video_fifo_high)
                         )
                    )
            {

                    if (m_av_clock->OMXIsPaused())
                    {
                        m_av_clock->OMXResume();
                        
                        _parent->sig_didResume();
                    }

            }
            
            // si en pause ou fifos audio et/ou video pleines
            else if ( m_Pause || audio_fifo_low || video_fifo_low)
            {
                if (!m_av_clock->OMXIsPaused())
                {
                    if (!m_Pause)
                        threshold = std::min(2.0f * threshold, 16.0f);
                    /*
                    printf("Pause %.2f,%.2f (%d,%d,%d,%d) %.2f\n",
                               audio_fifo,
                               video_fifo,
                               audio_fifo_low,
                               video_fifo_low,
                               audio_fifo_high,
                               video_fifo_high,
                               threshold
                              );
                    */
                    m_av_clock->OMXPause();
                    
                    _parent->sig_didPause();
                }
            }
            
            
        } // end of ShouldUpdate
        
        // on a renvoyé la commande 'play':
        // sentStarted = false => reset
        // sentStarted = true  => lecture 'normale'
        
        if ( !sentStarted )
        {
            m_av_clock->OMXReset(m_has_video, m_has_audio);

            sentStarted = true;
        }
        
        // paquet non NULL, on lit
        if(!m_omx_pkt)
        {
            m_omx_pkt = m_omx_reader.Read();
        }
        
        // un paquet lu, on n'est pas end-of-stream
        if(m_omx_pkt)
        {
            end_sent = false;
            if ( _firstPacket )
            {
                _parent->sig_didStart();
                _firstPacket = false;
            }
            send_eos = false;
        }
        
        // end-of-stream ET pas de paquet lu ,
        // on flag send_eos et on continue de lire s'il reste des paquets en attente
        if( m_omx_reader.IsEof() && !m_omx_pkt)
        {
            // demuxer EOF, but may have not played out data yet
            if (    ( m_has_video && m_player_video.GetCached() )
                 || ( m_has_audio && m_player_audio.GetCached() )
               )
            {

                OMXClock::OMXSleep(10);
                continue;
            }
            /*
            if (m_loop)
            {
                printf("\n Loop \n");
                m_incr = loop_from - (m_av_clock->OMXMediaTime() ? m_av_clock->OMXMediaTime() / DVD_TIME_BASE : last_seek_pos);
               // _parent->sig_didEnd();
                continue;
            }
            */


            if ( !send_eos )
            {
             
                if ( m_has_video )
                    m_player_video.SubmitEOS();
                
                if ( m_has_audio )
                    m_player_audio.SubmitEOS();
                
                _parent->sig_willEnd();

                end_sent = false;

                
            }
            
            send_eos = true;
            
            if (   (m_has_video && !m_player_video.IsEOS())
                || (m_has_audio && !m_player_audio.IsEOS()) )
            {
                OMXClock::OMXSleep(10);
                continue;
            }
        
            if ( end_sent == false)
            {
                end_sent = true;
                _parent->sig_didEnd();
            }


            if (m_loop)
            {

                m_incr = loop_from - (m_av_clock->OMXMediaTime() ? m_av_clock->OMXMediaTime() / DVD_TIME_BASE : last_seek_pos);
                continue;
            }

            
            /* **** **** **** */
            // END
            /* **** **** **** */
                
        }
        
        // si le fichier a de la vidéo, qu'il y a un paquet lu et qu'il est de nature video
        if(    m_has_video && m_omx_pkt
            && m_omx_reader.IsActive(OMXSTREAM_VIDEO, m_omx_pkt->stream_index)
          )
        {
            if (TRICKPLAY(m_av_clock->OMXPlaySpeed()) )
            {
                packet_after_seek = true;
            }
            
            // on envoie le paquet au player video et on le NULLise pour pouvoir écrire dedans à la prochaine étape
            if(m_player_video.AddPacket(m_omx_pkt))
            {
                m_omx_pkt = NULL;
            }
            
            else
                OMXClock::OMXSleep(10);
        }

        // si le fichier a de l'audio, qu'il y a un paquet lu et qu'il est de nature audio
        else if(    m_has_audio
                 && m_omx_pkt
                 && !TRICKPLAY(m_av_clock->OMXPlaySpeed())
                 && m_omx_pkt->codec_type == AVMEDIA_TYPE_AUDIO
                )
        {
            if(m_player_audio.AddPacket(m_omx_pkt))
            {
                m_omx_pkt = NULL;
            }
            else
                OMXClock::OMXSleep(10);
            
        }
/*
        // si le fichier a des sous-titres, qu'il y a un paquet lu et qu'il est de nature sous-titre
        else if(    m_has_subtitle
                 && m_omx_pkt
                 && !TRICKPLAY(m_av_clock->OMXPlaySpeed())
                 && m_omx_pkt->codec_type == AVMEDIA_TYPE_SUBTITLE)
        {
            auto result = m_player_subtitles.AddPacket(m_omx_pkt,
                                                       m_omx_reader.GetRelativeIndex(m_omx_pkt->stream_index));
            if (result)
                m_omx_pkt = NULL;
            
            else
                OMXClock::OMXSleep(10);
            
        }
*/
        
        // sinon on delete le paquet et on continue
        else
        {
            if(m_omx_pkt)
            {
                m_omx_reader.FreePacket(m_omx_pkt);
                m_omx_pkt = NULL;
            }
            
            else
                OMXClock::OMXSleep(10);
        }
        
    } // end of main loop
    
    /* **** **** **** **** **** **** **** **** **** */
    

    _parent->sig_didEnd();

    m_isRunning = false;
    
    return true;

    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

void MainPlayer::releasePlayer()
{
    if (m_stop)
    {
        unsigned t = (unsigned)(m_av_clock->OMXMediaTime()*1e-6);
        //printf("Stopped at: %02d:%02d:%02d\n", (t/3600), (t/60)%60, t%60);
    }
    
    
    m_av_clock->OMXStop();
    m_av_clock->OMXStateIdle();
    
    //m_player_subtitles.Close();
    m_player_video.Close();
    m_player_audio.Close();

    
    if(m_omx_pkt)
    {
        m_omx_reader.FreePacket(m_omx_pkt);
        m_omx_pkt = NULL;
    }
    
    m_omx_reader.Close();
    
    m_av_clock->OMXDeinitialize();



}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

void MainPlayer::run()
{

    if (!runInt())
    {
//        printf("\n Error while trying to play video \n");
    }

    releasePlayer();
    
}



