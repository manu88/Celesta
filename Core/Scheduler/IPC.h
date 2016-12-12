//
//  IPC.h
//  testIPC2
//
//  Created by Manuel Deneu on 29/06/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#ifndef __IPC__
#define __IPC__

#include "../Internal/Element.h"
#include "Dispatch.h"
#include "IPCImpl.h"

class IPCSender : public Element , private IPC_IMPL::Sender
{
    
public:
    
    IPCSender();
    virtual ~IPCSender();
    
    inline bool connectWithFile( const std::string &file )
    {
        return connect( file );
    }
    
    inline bool connectWithKey( key_t key )
    {
        return connect( key );
    }
    
    inline bool send( int type , const std::string &msg)
    {
        return sendMessage(msg , type);
    }
    
#ifdef ENABLE_ELEMENT_SELECTOR
    
    const Variant performSelectorWithArguments( const std::string &selector , const VariantList  &arguments, bool *validCall = nullptr );
    
#endif
    
};

/* **** **** **** **** **** **** **** **** **** **** **** */

class IPCListener : public Element ,public Dispatch::Task , private IPC_IMPL::Listener
{
    
public:
    
    IPCListener();
    virtual ~IPCListener();
    
    inline bool connectWithFile( const std::string &file )
    {
        if( connected() )
            return true;
        
        if( connect( file ) )
        {
            Dispatch::getInstance()->add(this );
            return true;
        }
        return false;
    }
    
    inline bool connectWithKey( key_t key )
    {
        if( connected() )
            return true;
        
        if( connect( key ) )
        {
            Dispatch::getInstance()->add(this );
            return true;
        }
        return false;
    }
    
    void doTask();
    
#ifdef ENABLE_ELEMENT_SELECTOR
    
    void setNotificationSelector( const std::string selector , Element *target)
    {
        _notificationSelector = selector;
        _notificationTarget = target;
    }
    
    const Variant performSelectorWithArguments( const std::string &selector , const VariantList  &arguments, bool *validCall = nullptr );
    
#endif
    
    
private:
    Element *_notificationTarget;
    std::string _notificationSelector;
};



#endif /* defined(__IPC__) */
