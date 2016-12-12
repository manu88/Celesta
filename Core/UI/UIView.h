//
//  UITarget.hpp
//  JSCenter
//
//  Created by Manuel Deneu on 23/11/2015.
//  Copyright © 2015 Manuel Deneu. All rights reserved.
//

#ifndef UITarget_hpp
#define UITarget_hpp

#include "../GX/GXScene.h"
#include "../GXTouch/GXTouch.h"


#include "UIViewController.h"

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

class UIView : public GXScene, public GXTouchDelegate
{
public:
    
#ifdef USE_JSON_PARSER
    static UIView* createUIElementFromJSON( const JSON::Node &node );
    bool saveJSON( JSON::Node &node) const;
#endif
    
    virtual ~UIView() {}
    
    void setViewController( UIViewController * controller)
    {
        _viewController = controller;
    }
    
    const UIViewController* getViewController() const
    {
        return _viewController;
    }

    void setTouchesEnabled( bool enabled)
    {
        _touchesEnabled = enabled;
    }
    
    bool touchesEnabled() const noexcept
    {
        return _touchesEnabled;
    }
    
    void setTarget(  const std::string &selector , Element *target )
    {
        _target = target;
        _selector = selector;
    }

    virtual void resetState();
//    virtual void setPosition( const GXPoint &pt);

    virtual bool toucheBegan( const std::list<GXTouch> &touches );
    virtual bool toucheMoved( const std::list<GXTouch> &touches );
    virtual bool toucheEnded( const std::list<GXTouch> &touches );
    
#ifdef ENABLE_ELEMENT_SELECTOR
    virtual const Variant performSelectorWithArguments( const std::string &selector , const VariantList  &arguments , bool *validCall =nullptr) ;
#endif
    
protected:
    UIView();
    
    
    virtual void viewWillAppear();
    virtual void viewDidAppear();
    virtual void viewWillDisappear();
    virtual void viewDidDisappear();
   
#ifdef USE_JSON_PARSER
    bool parseJSONBase( const JSON::Node &node );
    virtual bool saveJSONSpec( JSON::Node &node) const;
#endif
    
    bool _touchesEnabled;
    std::string _selector;
    Element    *_target;
    
    UIViewController *_viewController;
    
private:
    void elementWillAppear();
    void elementDidAppear();
    void elementWillDisappear();
    void elementDidDisappear();
    
    


};


#endif /* UITarget_hpp */
