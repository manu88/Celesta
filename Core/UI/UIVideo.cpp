//
//  UIVideo.cpp
//  JSCenter
//
//  Created by Manuel Deneu on 10/12/2015.
//  Copyright © 2015 Manuel Deneu. All rights reserved.
//

#include "UIVideo.h"
#include "UIJSON.h"
#include "../Env_Variables.h"

UIVideo::UIVideo():
_videoPlayer (new GXVideo() )
{
    className = ClassNames::UIVideo;
    
    addElement( _videoPlayer );
    
    _videoPlayer->setLayer( 0);
    
    setTransparency( true );
    
    setOwnsElements( false );
}

UIVideo::~UIVideo()
{
    delete _videoPlayer;
}

void UIVideo::setPosition( const GXPoint &pt)
{
    setBounds(makeRect(pt, getBounds().size ) );
    _videoPlayer->setBounds(getBounds() );
    
}

bool UIVideo::setVideoSource( const std::string &filename )
{
    return _videoPlayer->setFileSource( filename );
}

void UIVideo::viewDidAppear()
{
    _videoPlayer->setVisible(true);
}
void UIVideo::viewWillDisappear()
{
    _videoPlayer->setVisible(false);
}

#ifdef ENABLE_ELEMENT_SELECTOR
const Variant UIVideo::performSelectorWithArguments( const std::string &selector , const VariantList  &arguments , bool *validCall)
{
    if( validCall)
        *validCall = true;
    
    if( selector == "setVideoSource")
    {
        return Variant( setVideoSource( arguments.at(0).getString() ));
    }
    else if( selector == "start")
    {
        return Variant( start() );
    }
    else if( selector == "pause")
    {
        return Variant( _videoPlayer->pause() );
    }
    else if( selector == "resume")
    {
        return Variant( _videoPlayer->resume() );
    }
    else if( selector == "prepare")
    {
        return Variant( prepare() );
    }
    else if( selector =="setPercent")
    {
        const double percent = arguments.at(0).getDouble() ;
        
        const auto len = _videoPlayer->getVideoLength();
        
        const unsigned long  t = static_cast<unsigned long>( len.getInMs()* percent );

        _videoPlayer->seekToTC(Timecode(0,0,0,t));
        
        return Variant::null();
    }
    else if( selector == "setAlpha")
    {
        _videoPlayer->setAlpha( static_cast<uint8_t>( arguments.at(0).getInt() ) );
        
        return Variant::null();
    }
    else if( selector =="getVideoPlayer")
    {
        return  Variant( _videoPlayer->getElementId() );
    }
        
    if( validCall )
        *validCall = false;
    
    return UIView::performSelectorWithArguments(selector, arguments, validCall);
}
#endif

#ifdef USE_JSON_PARSER
/*static*/ UIVideo* UIVideo::createFromJSON( const JSON::Node &node )
{
    if( node.isInvalid() )
        return nullptr;
    
    UIVideo *vid = new UIVideo();
    
    if( node.getObjectItem( UIAttribute::Ressource ).isString() )
        vid->setVideoSource( node.getObjectItem(UIAttribute::Ressource).getString() );
    
    return vid;
}

bool UIVideo::saveJSONSpec( JSON::Node &node) const
{
    node.addItemToObject( Variant(_videoPlayer->getFileSource() ), UIAttribute::Ressource );

    node.addItemToObject( Variant({ Variant(_videoPlayer->getBounds().size.width),
                                    Variant(_videoPlayer->getBounds().size.height) }),
                         UIAttribute::Size
                         );
    return true;
}
#endif

