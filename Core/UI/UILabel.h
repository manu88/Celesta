//
//  UILabel.hpp
//  JSCenter
//
//  Created by Manuel Deneu on 27/11/2015.
//  Copyright © 2015 Manuel Deneu. All rights reserved.
//

#ifndef UILabel_hpp
#define UILabel_hpp

#include "../GX/GXAsyncPainter.h"
#include "../GX/GXTextRenderer.h"

#include "UIView.h"

class UILabel : public UIView
{
public:
    UILabel();
    ~UILabel();
    
    void setText( const std::string &text );
    void setPosition( const GXPoint &pt);
    
#ifdef ENABLE_ELEMENT_SELECTOR
    const Variant performSelectorWithArguments( const std::string &selector , const VariantList  &arguments , bool *validCall = nullptr) ;
#endif
    
protected:

    
private:
    GXTextRenderer *_text;
};

#endif /* UILabel_hpp */
