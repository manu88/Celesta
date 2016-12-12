//
//  Controller.h
//  JSCenter
//
//  Created by Manuel Deneu on 08/07/2015.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#ifndef __JSCenter__Controller__
#define __JSCenter__Controller__
#include <map>
#include <stdio.h>
#include "../Core/Core.h"

namespace ClassNames
{
    static const std::string JSController  = "JSController";
}


class Controller : public ApplicationBase,
                   public JSMachineDelegate,
                   public InterfaceControllerDelegate,
                   public EventListener,
                   public WebServerDelegate,
                   public GXTouchDelegate,
                   public UIViewController
//                   public AttributeObserver

{
public:
    /*
    void valueChanged( const AttributesList* object, const std::string &attribute, const ValueState &state)
    {
        const Element *el = dynamic_cast<const Element*>(object);
        if( state == ValueAdded)
            printf("Value Added in Element %s for attribute %s\n " ,el->getElementName().c_str(), attribute.c_str() );
        
        else if (state == ValueChanged )
            printf("Value Changed in Element %s for attribute %s\n " ,el->getElementName().c_str(), attribute.c_str() );
        else if( state == ValueRemoved )
            printf("Value Removed in Element %s for attribute %s\n " ,el->getElementName().c_str(), attribute.c_str() );            
    }
    */
    Controller( const std::string &configFile);
    ~Controller();
    
    
    void setOscCallback( const std::string &str)
    {
        _netCallback = str;
    }
    
    void setInterfaceCallback( const std::string &str)
    {
        _interfaceCallback = str;
    }
    

    
    
protected:
    bool applicationWillStart();
    void applicationDidStart();
    void applicationWillStop();
    void applicationDidStop();
    
    void viewWillAppear   ( UIView *view );
    void viewDidAppear    ( UIView *view );
    void viewWillDisappear( UIView *view );
    void viewDidDisappear ( UIView *view);
    
    bool reload();
    

    void initElementClass( const std::string &className );
    
    Element *createElement( const std::string &name);
    
    bool deleteElement( Element::element_id id )
    {
        Element* e = Element::getElementById( id );
        
        if (e)
            delete e;
        
        return e != nullptr;
    }
    
    void functionCalled( const Selector *selector );
    void oscReceived(const std::string &ipAddress , const int port,const std::string & addressPattern, const VariantList &arguments);
    void inputChanged( const InterfaceEvent *event );
    void eventReceived( const Event *event);
    std::string getRequest( const std::string &ipAddress ,
                           const int port,
                           const std::string &addressPattern,
                           const Database &arguments);
    
    /*
    void toucheBegan( const GXPoint &pt );
    void toucheMoved( const GXPoint &pt);
    void toucheEnded( const GXPoint &pt);
    */

    void postMessage( const std::string &msg);
    
    /* JS Timer wrapper */
    class TimerHandler : public Object
    {
    public:
        TimerHandler( Controller* ctlr , const std::string &callback):
        _ctlr( ctlr ),
        _callback( callback )
        {}
        
        ~TimerHandler()
        {}
        
        void onTime();
    private:
        Controller *_ctlr;
        std::string _callback;
        
    };
    
    /* *** *** *** *** */



    void onTime( const std::string &callback);
    Dispatch::timer_id createTimer( const Timecode &delay , const Timecode &period , const std::string &callback);
    bool removeTimer( const Dispatch::timer_id id );
    
#ifdef ENABLE_ELEMENT_SELECTOR
    const Variant performSelectorWithArguments( const std::string &selector , const VariantList  &arguments , bool *validCall = nullptr)
    {

        bool valid = false;
        Variant ret;
        ret = ApplicationBase::performSelectorWithArguments(selector, arguments, &valid);
        
        if( valid)
        {
            if( validCall)
                *validCall = valid;
            
            return ret;
        }

        else
        {
            valid = true;

            valid = _js.executeBuffer( JSMachine::getCallFromSelector(selector, arguments ) );
            
            if (validCall)
                *validCall = valid;
        }

        return Variant::null();
    }
#endif
    
    
private:

    JSMachine _js;
    


    std::string         _scriptFile;
    
    std::string         _msgCallback;
    std::string         _netCallback;
    
    std::string         _interfaceCallback;
    std::string         _displayCallback;
    
    std::string         _webCallback;
    std::string         _serialCallback;
    
    std::vector<TimerHandler*> _timers;
    
};

#endif /* defined(__JSCenter__Controller__) */
