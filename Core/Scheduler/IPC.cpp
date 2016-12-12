
#include "IPC.h"

IPCSender::IPCSender()
//IPC_IMPL::Sender("/Users/manueldeneu/Documents/projets/dev/IPC/IPCServer/IPCClient/main.cpp")
{
    setElementName(ClassNames::IPCSender );
    className = ClassNames::IPCSender;
    
    addTypeInfo( Network );
    addTypeInfo( Output);
}

IPCSender::~IPCSender()
{
    
}

#ifdef ENABLE_ELEMENT_SELECTOR
const Variant IPCSender::performSelectorWithArguments( const std::string &selector , const VariantList  &arguments, bool *validCall )
{
    if( validCall)
        *validCall = true;
    
    if( selector == "send")
    {
        return Variant( send( arguments.at(0).getInt() , arguments.at(1).getString() ));
    }
    
    else if( selector == "connectWithFile")
    {
        return Variant( connectWithFile( arguments.at(0).getString() ) );
    }
    else if( selector == "connectWithKey")
    {
        return Variant( connectWithKey( arguments.at(0).getInt() ) );
    }
    
    else if( selector == "isConnected")
    {
        return Variant( connected() );
    }
    
    if( validCall)
        *validCall = false;
    
    return Element::performSelectorWithArguments(selector, arguments, validCall);
}
#endif

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

IPCListener::IPCListener():
Dispatch::Task(100 , ClassNames::IPCListener ),
//IPC_IMPL::Listener("/Users/manueldeneu/Documents/projets/dev/IPC/IPCServer/IPCClient/main.cpp"),

_notificationTarget( nullptr )
{
    setElementName( ClassNames::IPCListener );
    className = ClassNames::IPCListener;

    addTypeInfo( Network );
    addTypeInfo( Input );

}

IPCListener::~IPCListener()
{
    
}

void IPCListener::doTask()
{
    if( !connected() )
        return;
    
    struct msgBuffer msg = { -1 , { "Undefined" }};
    
    if( readMsg( &msg) )
    {
        if( _notificationTarget )
        {
            _notificationTarget->performSelectorWithArguments(_notificationSelector, {
                                                        Variant( getElementId() ),
                                                        Variant( msg.mtype ),
                                                        Variant( msg.info.name )
                                                                                     }
                                                              );
        }
    }
}

#ifdef ENABLE_ELEMENT_SELECTOR
const Variant IPCListener::performSelectorWithArguments( const std::string &selector , const VariantList  &arguments, bool *validCall )
{
    if( validCall)
        *validCall = true;
    
    if( selector == "getTypeToRead")
        return Variant( getTypeToRead() );
    
    else if( selector == "setTypeToRead")
    {
        setTypeToRead( arguments.at(0).getLong() );

        return  Variant::null();
    }
    
    else if( selector == "connectWithFile")
    {
        return Variant( connectWithFile( arguments.at(0).getString() ) );
    }
    else if( selector == "connectWithKey")
    {
        return Variant( connectWithKey( arguments.at(0).getInt() ) );
    }
    
    else if( selector == "isConnected")
    {
        return Variant( connected() );
    }
    
    else if ( selector == "setNotificationSelector")
    {
        _notificationSelector = arguments.at(0).getString();
        _notificationTarget = Element::getElementById( arguments.at(1).getUInt64() );
        
        return Variant( _notificationTarget != nullptr );
    }
    
    if( validCall)
        *validCall = false;
    
    return Element::performSelectorWithArguments(selector, arguments, validCall);
}
#endif


