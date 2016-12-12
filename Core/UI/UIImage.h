//
//  UIImage.hpp
//  JSCenter
//
//  Created by Manuel Deneu on 27/11/2015.
//  Copyright © 2015 Manuel Deneu. All rights reserved.
//

#ifndef UIImage_hpp
#define UIImage_hpp

#include "UIView.h"
#include "../GX/GXImage.h"

class UIImage : public UIView
{
public:
    
    UIImage();
    ~UIImage();
    
    void setPosition( const GXPoint &pt);
    bool setImageSource( const std::string &filename );
    
#ifdef ENABLE_ELEMENT_SELECTOR
    const Variant performSelectorWithArguments( const std::string &selector , const VariantList  &arguments , bool *validCall = nullptr) ;
#endif
    
#ifdef USE_JSON_PARSER
    static UIImage* createFromJSON( const JSON::Node &node );
#endif
    
protected:
#ifdef USE_JSON_PARSER
    bool saveJSONSpec( JSON::Node &node) const;
#endif
    
private:
    GXImage *_image;
};

#endif /* UIImage_hpp */
