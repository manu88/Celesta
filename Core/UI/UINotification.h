//
//  UINotification.hpp
//  JSCenter
//
//  Created by Manuel Deneu on 27/11/2015.
//  Copyright © 2015 Manuel Deneu. All rights reserved.
//

#ifndef UINotification_hpp
#define UINotification_hpp

#include "UIView.h"
#include "../Env_Variables.h"

class UIWindow;
class UILabel;
class UIButton;
class GXAsyncPainter;

class UIModalAlert : public UIView
{
public:
    
    static int showModalAlert(UIWindow *parent , const std::string &title );
    
    UIModalAlert( UIWindow* parent);
    ~UIModalAlert();
    
    int show();
    
    void setTitle( const std::string &title);
    

    
#ifdef ENABLE_ELEMENT_SELECTOR
    static const Variant staticSelector(const std::string& selector , const VariantList& arguments, bool *validCall);
    
    static void init()
    {
        Element::addStaticCallbackForClassName( staticSelector, ClassNames::UIModalAlert );
    }
    
    const Variant performSelectorWithArguments( const std::string &selector , const VariantList  &arguments , bool *validCall = nullptr) ;
    
#endif
protected:
    bool create();
private:
    UIWindow *_window;
    
    GXAsyncPainter *_rect;
    UILabel  *_title;
    UIButton *_okButton;
    UIButton *_cancelButton;
    
    int  _ret;
    bool _shouldReturn;
    
    GXRect _winBounds;
};

#endif /* UINotification_hpp */
