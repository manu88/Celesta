/*!
 *  \brief     Network Controller
 *  \details   This class is used to controller network Events such as Osc and Udp packets
 *  \author    Manuel Deneu
 *  \version   0
 *  \date      2014
 *  \pre       .
 *  \bug       Broadcast messages don't work on Unix( tested on RPI only)
 *  \warning   .
 *  \copyright GNU Public License.
 */

#ifndef ____NetworkController__
#define ____NetworkController__

#include <iostream>
#include <unordered_set>
#include <set>
#include <algorithm>

#include "../Log/Log.h"
#include "../Config.h"

#include <oscpack/osc/OscReceivedElements.h>
#include <oscpack/osc/OscPacketListener.h>
#include <oscpack/osc/OscOutboundPacketStream.h>
#include <oscpack/ip/UdpSocket.h>
#include <oscpack/ip/PacketListener.h>


#include "../Internal/Object.h"

#include "../Internal/AbstractDelegate.h"

#include "../Scheduler/Event.h"

#include "../Internal/AbstractController.h"





/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class NetworkController;     // forward
class NetworkControllerLock; // forward


class NetworkControllerDelegate : public virtual Object , public  AbstractDelegate
{
    friend class NetworkController;
public:
    virtual ~NetworkControllerDelegate() {}
    
    
protected:
    NetworkControllerDelegate() {}
    
    virtual void oscReceived(const std::string &ipAddress , const int port,const std::string & addressPattern, const VariantList &arguments) = 0;

};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */


class SocketUdpIn : public UdpSocket , private Object
{
public:
    SocketUdpIn( int port) :
        _port( port )
    {
        className = "SocketUdpIn";
        SetAllowReuse( true );
    }
    
    ~SocketUdpIn()
    {}
    
    int getPort() const noexcept
    {
        return _port;
    }
    
private:
    int _port;
    
    
};

class EventOscIn : public Event
{
public:
    EventOscIn( const std::string &_ip , int _port , const std::string & _address , const VariantList &_arguments , Element *sender ):
    Event(Event_OSC , sender),
    ip( _ip),
    port( _port),
    address( _address),
    arguments( _arguments )
    {
        className = "EventOscIn";
    }
    
    ~EventOscIn()
    {}
    
    std::string ip;
    int         port;
    std::string address;
    VariantList arguments;
};


class NetworkController : public Element,
                          public AbstractController,
                          public Thread,
                          public osc::OscPacketListener
{
public:
    NetworkController();
    virtual ~NetworkController();
    

    void setDelegate( NetworkControllerDelegate *delegate) noexcept
    {
        _delegate = delegate;
    }
    
    // if set to true, incomming osc messages will be passed as Dispatch Messages.
    void setAsyncCallback( bool async ) noexcept
    {
        _asyncCallback = async;
    }
    
    bool isAsync() const noexcept
    {
        return _asyncCallback;
    }

    friend class NetworkControllerLock;

    
    //! Starts the internal thread.
    bool start();
    bool stop();
    
    bool sendOSC( const std::string &ip , const int port , const std::string &address , const VariantList &vars = {} , bool broadcast = false )  noexcept;
    
    bool addPort    ( int port );
    bool removePort ( int port );
    
    void removeAllSockets();
    
    void ProcessMessage( const osc::ReceivedMessage& m, const IpEndpointName& remoteEndpoint );
    
    inline static VariantList getVariantListFromOSCMsg( const osc::ReceivedMessage& m )
    {
        VariantList ret;
        
        try
        {

            for (auto it = m.ArgumentsBegin() ; it != m.ArgumentsEnd() ; it++ )
            {
                if ( (it)->IsFloat() )
                {
                    ret.push_back( Variant ( (it)->AsFloat() ) );
                }
                
                else if ( (it)->IsInt32() )
                {
                    ret.push_back( Variant ( (int) (it)->AsInt32() ));
                }
                
                else if ( (it)->IsInt64() )
                {
                    ret.push_back( Variant ( (int) (it)->AsInt64()) );
                }
                
                else if ( (it)->IsString() )
                {
                    char chars[] = "\"";
                    std::string str = (it)->AsString();
                    for (unsigned int i = 0; i < strlen(chars); ++i)
                    {
                        str.erase (std::remove(str.begin(), str.end(), chars[i]), str.end());
                    }
                    ret.push_back( Variant ( str ) );
                }
                else
                {
                    // No parsing method for this type, to be added ...
                    DEBUG_ASSERT( false );
                }

            }
            
        }
        
        catch( osc::Exception& e )
        {
            Log::log("\n error while parsing message: %s " , e.what() );
        }

        
        return ret;
    }
    
    
#ifdef ENABLE_ELEMENT_SELECTOR
    const Variant performSelectorWithArguments( const std::string &selector , const VariantList  &arguments , bool *validCall = nullptr) ;
#endif

    
private:
    
    enum { Output_Buffer_Size = 1024  };
    
    SocketUdpIn* getSocketForPort( int port ) const noexcept;
    void internalDeleteSocket( SocketUdpIn* socket );
    
    void lockThread()  noexcept
    {
        _lockThread = true;
    }
    
    void unlockThread() noexcept
    {
        _lockThread = false;
    }
    
    void run();
    
    NetworkControllerDelegate   *_delegate;

    std::string                  _oscCallbackSelector;
    Element                     *_oscCallbackTarget;
    volatile bool _lockThread;
    
    SocketReceiveMultiplexer _multiplexer;

    std::unordered_set<SocketUdpIn*> _socketList;
    
    bool _asyncCallback;
    

};

static inline osc::OutboundPacketStream& operator<<( osc::OutboundPacketStream& p, const Variant& val)
{
    if ( val.isNull() )
    {}
    else if ( val.isInt())
        p << val.getInt();
    
    else if ( val.isInt64() )
        p<< ( osc::int64 ) val.getInt64();
    
    else if ( val.isString() )
        p << val.getString().c_str();
    
    else if ( val.isFloat() )
        p << val.getFloat();
    
    else if ( val.isDouble() )
        p << (float) val.getDouble();
    
    
    else if ( val.isBool() )
    {
        p << val.getBool();
    }
    
    else if ( val.isList() )
    {
        for ( const Variant &v : val.getList() )
            p << v;
    }
    
    return p;

}

namespace NetworkTools
{
//    const std::unique_ptr< Variant > getArrayFromOSCMessage(const osc::ReceivedMessage& m);
    bool changeIpAddressTo(const std::string ip , const std::string netmask , const std::string interface );

    const std::set< std::string > getInterfacesList();
    const std::string getIpAddress( const std::string &interfaceName, bool ipV6 = false);

    const std::string getHostName();
    bool isConnected();
};

class NetworkControllerLock
{
public:
    NetworkControllerLock( NetworkController *controller ):
    _controller ( controller )
    {
        _controller->lockThread();
        _controller->_multiplexer.AsynchronousBreak();
    }
    
    ~NetworkControllerLock()
    {
        _controller->unlockThread();
    }
private:
    NetworkController* _controller;
};


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class OscLogger : public Log
{
public:
    
    OscLogger( NetworkController *controller,  const std::string &ip , unsigned int port):
    _ip         ( ip   ),
    _port       ( port ),
    _controller ( controller )
    {
        
    }
    
    ~OscLogger()
    {
        
    }
    
protected:
    void print( const char * c  ) const
    {

        std::string str(c);
        str.erase( std::remove_if( str.begin(),
                                   str.end(),
                                       [](char cc)
                                        {
                                           switch( cc )
                                           {
                                               case '\t':
                                               case '\n':
                                                   return true;
                                               default:
                                                   return false;
                                           }
                                        }
                                  ),
                                    str.end());
        _controller->sendOSC( _ip, _port, "/log" ,{ Variant ( str ) } );
        
    }
    
private:
    std::string        _ip;
    unsigned int       _port;
    NetworkController *_controller;
};




#endif /* defined(____NetworkController__) */
