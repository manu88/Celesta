//
//  GXVideo.cpp
//  
//
//  Created by Manuel Deneu on 13/08/14.
//
//

#include "GXVideo.h"
#include "../Log/Log.h"

#include "../Internal/Thread.h"
#include "../Scheduler/Dispatch.h"

#include "../Env_Variables.h"

VideoEvent::VideoEvent( GXVideo *video , GXVideoNotification notif ):
Event( Event_Video , video),
notification( notif)
{}

GXVideo::GXVideo() :

_notifTarget(nullptr ),

_player     ( this   ),
m_isPrepared ( false  ),
m_length     ( Timecode() )
{
    className = ClassNames::GXVideo;
    
    // no need to access vars on GUI thread
    performChangedSignalOnGUIThread( false );
    
    _player.setClock( &m_clock );
    
    setVisible( true);
    
    setElementName( ClassNames::GXVideo );
    
    addTypeInfo( Audio);
    

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

GXVideo::~GXVideo()
{
    stop();
    
    if (m_isPrepared )
        releasePlayer();

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXVideo::prepareRessources()
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXVideo::deleteRessources()
{
    if ( isStarted() )
        stop();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
void GXVideo::setBounds( const GXRect &bounds )
{
    
    const GXSize dispSize = DisplayController::getInstance()->getCurrentMode().size;
    
    
    _player.setBounds( bounds.origin.x,
                       dispSize.height - (bounds.size.height + bounds.origin.y),
                       bounds.size.width,
                       bounds.size.height
                      );

    GXElement::setBounds( bounds );

}

void GXVideo::setLayer(int layer)
{
    GXElement::setLayer( layer );
    
    _player.setLayerNum( getLayer() );
}
void GXVideo::changed()
{
//    _player.setBounds(getBounds().origin.x, getBounds().origin.y,  getBounds().size.width, getBounds().size.height);
    
/*    _player.setLayerNum( getLayer() );
    
*/
    _player.flipVisibilityTo( isVisible() );
    

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXVideo::paint( const GXRect & , GXAnimation* )
{

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXVideo::setAudioOutput( const AUDIO_OUTPUT &output)
{
    m_audioOutput = output;

    
    if (m_audioOutput == AUDIO_BOTH)
        _player.setAudioDeviceName("omx:both");
    
    else if (m_audioOutput == AUDIO_HDMI)
        _player.setAudioDeviceName("omx:hdmi");
    
    else if (m_audioOutput == AUDIO_LOCAL)
        _player.setAudioDeviceName("omx:local");
    
    

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

Timecode GXVideo::getVideoLength()
{
    const int dur = _player.getStreamLength();

    m_length.h     = static_cast<unsigned long>( dur/3600 );
    m_length.min   = static_cast<unsigned long>( (dur/60)%60 ) ; // dur/3600;
    m_length.sec   = static_cast<unsigned long>( dur%60 );
    m_length.milli = 0;

    return m_length;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXVideo::seekToTC( Timecode tc )
{
    if ( tc > m_length )
        return;
    
    double tcInS = tc.getInMs() / 1000.0f;
    
    _player.seekTo( tcInS );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXVideo::setSpeed( float speed )
{
    _player.setSpeedAsync( (int ) speed );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

float GXVideo::getSpeed() const
{
    return 1.0f;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXVideo::registerTCNotification( const Timecode &tc )
{
    _player.registerTC (tc.getInMs() );
}

void GXVideo::resetAllTCNotification()
{
    _player.resetAllRegisteredTC();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool GXVideo::changeFileTo( const std::string &file )
{
    willChange();
    
    Thread::sleepForMs(10);

    releasePlayer();
    Thread::sleepForMs(10);
    setFileSource( file  );
    
    Thread::sleepForMs(10);
    if (!prepare() )
        Log::log("PREPARE VIDEO IN CHANGE FILE ___PAS____ OK \n");
    
    Thread::sleepForMs(10);
    
    didChange();
    
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
    Callback from MainPlayer to SchedulerDelegate
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** */


void GXVideo::sendNotification( GXVideoNotification notif )
{
    _notif = notif;
    if( _notifTarget )
    {
        _notifTarget->performSelectorWithArguments( _notifSelector, { Variant( _notif )});
    }
    else
        Dispatch::getInstance()->pushNotification( new VideoEvent(this , _notif ) );
}
void GXVideo::sig_ready()
{
    sendNotification( VideoLoaded );
}

void GXVideo::sig_willStart()
{
    sendNotification( VideoWillStart );
}

void GXVideo::sig_didStart()
{
    sendNotification( VideoDidStart );
}

void GXVideo::sig_didPause()
{
    sendNotification( VideoPaused );
}

void GXVideo::sig_didResume()
{
    sendNotification( VideoDidResume );
}

void GXVideo::sig_willEnd()
{
    sendNotification( VideoWillEnd );
}
void GXVideo::sig_didEnd()
{
    sendNotification( VideoDidEnd );
}

void GXVideo::sig_didReachTC( unsigned long /*millis*/)
{
    sendNotification( VideoDidReachTC );
}

#ifdef ENABLE_ELEMENT_SELECTOR
const Variant GXVideo::performSelectorWithArguments( const std::string &selector , const VariantList  &arguments , bool *validCall /*= nullptr*/)
{
    if ( validCall )
        *validCall = true;
    
    if( selector == "getCurrentTC")
        return Variant (  getCurrentTC().getInMs() ) ;
    
    else if( selector == "getRemainingTC")
        return Variant( getRemainingTC().getInMs() );
    
    else if( selector == "getVideoLength")
        return Variant(  getVideoLength().getInMs() );
    
    else if( selector == "setFileSource")
        return Variant ( setFileSource( arguments.at(0).getString() ) );
    
    else if( selector == "getFileSource")
        return Variant ( getFileSource() );
    
    else if( selector == "changeFileTo")
        return Variant ( changeFileTo( arguments.at(0).getString() ));
    
    else if( selector == "setLooped")
    {
        setLooped( arguments.at(0).getBool() );
        return Variant::null();
    }
    else if( selector == "isLooped")
    {
        return  Variant( isLooped() );
    }
    else if( selector == "seekToTC" )
    {
        seekToTC( Timecode( arguments.at(0).getULong() ,
                            arguments.at(1).getULong() ,
                            arguments.at(2).getULong() ,
                            arguments.at(3).getULong()
                           )
                 );
        return Variant::null();
    }
    
    else if ( selector == "setSpeed")
    {
        setSpeed( arguments.at(0).getFloat() );
        return Variant::null();
    }
    
    else if( selector == "prepare")
        return Variant(prepare());
    
    else if( selector == "start")
        return Variant ( start() );
    
    else if( selector == "pause")
        return Variant( pause() );
    
    else if( selector == "resume")
        return Variant( resume() );
    
    else if( selector == "stop")
    {
        stop();
        return Variant::null();
    }
    
    else if ( selector == "release")
    {
        releasePlayer();
        return Variant::null();        
    }
    
    else if( selector == "isStarted")
        return Variant ( isStarted() );
    
    else if( selector == "isPaused")
        return Variant ( isPaused() );
    
    else if( selector == "setVolume")
    {
        setVolume( arguments.at(0).getFloat() );
        return Variant::null();
    }
    else if( selector == "getVolume")
    {
        return Variant( getVolume() );
    }
    else if ( selector == "setNotificationCallback")
    {
        _notifSelector = arguments.at(0).getString();
        _notifTarget   = Element::getElementById( arguments.at(1).getUInt64() );
        
        return Variant( _notifTarget != nullptr );
    }
    
    if ( validCall )
        *validCall = false;
    
    
    return GXElement::performSelectorWithArguments(selector, arguments , validCall );
}

#endif
