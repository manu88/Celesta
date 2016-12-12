//
//  UIVideo.hpp
//  JSCenter
//
//  Created by Manuel Deneu on 10/12/2015.
//  Copyright © 2015 Manuel Deneu. All rights reserved.
//

#ifndef UIVideo_hpp
#define UIVideo_hpp

#include "UIView.h"

#include "../GXExtra/GXVideo.h"

class UIVideo : public UIView
{
public:
    
    UIVideo();
    ~UIVideo();
    
    void setPosition( const GXPoint &pt);
    
    bool setVideoSource( const std::string &filename );
    
    inline bool start()
    {
        return _videoPlayer->start();
    }
    
    inline bool prepare()
    {
        return _videoPlayer->prepare();
    }
    
#ifdef ENABLE_ELEMENT_SELECTOR
    const Variant performSelectorWithArguments( const std::string &selector , const VariantList  &arguments , bool *validCall = nullptr) ;
#endif
    
#ifdef USE_JSON_PARSER
    static UIVideo* createFromJSON( const JSON::Node &node );
#endif
    
protected:
    
#ifdef USE_JSON_PARSER
    bool saveJSONSpec( JSON::Node &node) const;
#endif
    

    void viewDidAppear();
    void viewWillDisappear();
    
    
private:
    GXVideo *_videoPlayer;
    
};

#endif /* UIVideo_hpp */
