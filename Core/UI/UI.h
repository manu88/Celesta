//
//  UI.hpp
//  JSCenter
//
//  Created by Manuel Deneu on 23/11/2015.
//  Copyright © 2015 Manuel Deneu. All rights reserved.
//

#ifndef UI_hpp
#define UI_hpp

#include <stdio.h>

#include "../Internal/Element.h"
#include "../GX/GXScene.h"


#include "../GXTouch/GXTouch.h"


#include "UIView.h"
#include "UIButton.h"
#include "UISlider.h"
#include "UIImage.h"
#include "UILabel.h"
#include "UINotification.h"
#include "UIViewController.h"
#include "UIVideo.h"

#include "UIJSON.h"


#ifdef USE_JSON_PARSER
#include "../Parsers/JSONParser.h"
#endif

/* **** **** **** **** **** **** **** **** **** **** **** **** */


/* **** **** **** **** **** **** **** **** **** **** **** **** */

namespace UIDefaults
{
    static const GXColor BackgroundColor = {110 , 100 , 110, 255};
}

/* **** **** **** **** **** **** **** **** **** **** **** **** */

class UIWindow : public UIView
{
public:
    UIWindow();
    ~UIWindow();
    
    void setToDefaultValues();
    
#ifdef USE_JSON_PARSER
    bool parseJSON( const JSON::Node &node);
    bool saveJSON( JSON::Node &node) const;
    
    static UIWindow* createFromJSON( const JSON::Node &node);
#endif
    

    void viewWillAppear();
    void viewDidAppear();
    void viewWillDisappear();
    void viewDidDisappear();
    
#ifdef ENABLE_ELEMENT_SELECTOR
    const Variant performSelectorWithArguments( const std::string &selector , const VariantList  &arguments , bool *validCall = nullptr) ;
#endif
    
private:
    
};

/* **** **** **** **** **** **** **** **** **** **** **** **** */

class UIStoryBoard : public Element
{
public:
    UIStoryBoard();
    ~UIStoryBoard();
    
    void addWindow(UIWindow* win)
    {
        _windows.push_back( win );
        win->Element::setParentElement(this );
        
        updateValueForKey( UIAttribute::Children, Variant( getNumWindows() ));
    }
    
    const std::list<UIWindow*> &getWindows() const
    {
        return _windows;
    }
    
    size_t getNumWindows() const noexcept
    {
        return _windows.size();
    }
    
    UIWindow* getWindowNamed( const std::string &name)
    {
        const auto it =find_if(_windows.begin(), _windows.end(), [&name](const UIWindow* win)
                                                   {
                                                    return win->getElementName() == name;
                                                   });
        
        if( it!=_windows.end() )
            return (*it);
        
        return nullptr;
    }
    
#ifdef USE_JSON_PARSER
    bool parseJSON( const JSON::Node &node);
    bool saveJSON( JSON::Node &node) const;
#endif

    
#ifdef ENABLE_ELEMENT_SELECTOR
    const Variant performSelectorWithArguments( const std::string &selector , const VariantList  &arguments, bool *validCall = nullptr ) ;
#endif
    
private:
    std::list<UIWindow*> _windows;
};

#endif /* UI_hpp */
