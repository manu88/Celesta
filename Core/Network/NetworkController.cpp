//
//  NetworkController.cpp
//  
//
//  Created by Manuel Deneu on 14/08/14.
//
//


#include <assert.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <errno.h>

#include <string>
#include <algorithm>

/**/

// for ip stuffs
#include <sys/types.h>
#include <ifaddrs.h>

#include <arpa/inet.h>

#include <sys/param.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <sys/time.h>

#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>

#include <unistd.h>
#include <stdio.h>
#include <ctype.h>


#include <stdlib.h>
#include <stdint.h>
#include <iostream>
/**/

#include "NetworkController.h"


#include <oscpack/osc/OscTypes.h>

#include <oscpack/osc/OscException.h>

#include <oscpack/ip/UdpSocket.h>

#include "../Log/Log.h"
#include "../Config.h"

#include "../Scheduler/Dispatch.h"
#include "../Env_Variables.h"
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

NetworkController::NetworkController() :
AbstractController(ClassNames::NetworkController ),
Thread(ClassNames::NetworkController),

_oscCallbackSelector(""),
_oscCallbackTarget( nullptr ),
_asyncCallback ( false )

{
    setElementName( ClassNames::NetworkController );
    className = ClassNames::NetworkController;
    
    addTypeInfo( Input );
    addTypeInfo( Output );
    addTypeInfo( Network );
}

NetworkController::~NetworkController()
{
    removeAllSockets();
    stop();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool NetworkController::start()
{

    unlockThread();
    return startThread();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool NetworkController::stop()
{
    _lockThread = false;
    sendAsyncStop();
    
    _multiplexer.AsynchronousBreak();
    
    return stopThread();

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void NetworkController::run()
{
    while ( !threadShouldStop() )
    {
        setReady();
        
        // doesn't return until m_multiplexer.AsynchronousBreak(); is called
        // see NetworkControllerLock 
        _multiplexer.Run();

        if ( threadShouldStop() )
            break;
        
        while ( _lockThread )
        {
            if ( threadShouldStop() )
                break;
        }

    }

    setUnReady();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool NetworkController::addPort(int port)
{
    NetworkControllerLock l(this);
    
    if ( getSocketForPort( port ) != nullptr )
    {
        Log::log("\n socket for port %i already exists" , port);
        return true;
    }

    try
    {
        SocketUdpIn *socket = new SocketUdpIn( port );
        _socketList.insert( socket );
        

        
        socket->Bind( IpEndpointName( IpEndpointName::ANY_ADDRESS, port ) );
        _multiplexer.AttachSocketListener( socket, this );
    }
    catch (...)
    {

        Log::log(" unable to bind socket!");
        return false ;
    }

    

    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool NetworkController::removePort ( int port )
{
    SocketUdpIn *socket = getSocketForPort( port );
    
    if ( socket )
    {
        NetworkControllerLock l(this);
        

        internalDeleteSocket( socket );
        
        _socketList.erase( socket );
        
        return true;
    }
    
    return false;

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void NetworkController::internalDeleteSocket( SocketUdpIn* socket )
{
    _multiplexer.DetachSocketListener( socket , this );
    
    delete socket;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void NetworkController::removeAllSockets()
{
    NetworkControllerLock l(this);
    for (auto i : _socketList )
    {
        internalDeleteSocket( i );
    }
    
    _socketList.clear();
    

}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

SocketUdpIn* NetworkController::getSocketForPort( int port ) const noexcept
{
    for (auto i : _socketList )
    {
        if ( i->getPort() == port )
            return  i;
    }
    
    return nullptr;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */


bool NetworkController::sendOSC( const std::string &ip ,
                                 const int port ,
                                 const std::string &address ,
                                 const VariantList &vars /*= {}*/ ,
                                 bool broadcast /*false*/)  noexcept
{
    try
    {    

        UdpSocket transmitSocket;
        
        transmitSocket.SetEnableBroadcast( broadcast );        
        transmitSocket.Connect( IpEndpointName( ip.c_str() , port ));
        

        char buffer[ Output_Buffer_Size ];
        
        osc::OutboundPacketStream p( buffer, Output_Buffer_Size );
        
        p << osc::BeginMessage( address.c_str() );
    

        for ( const Variant &v : vars)
            p << v;
        


        p<< osc::EndMessage;
        
        transmitSocket.Send( p.Data(), p.Size() );

        return true;
        
    }
    catch ( const osc::Exception &excep/*  ... */)
    {
//        Log::log("\n caught exception on Osc::send : %s" , excep.what() );
    }
    
    catch ( ... )
    {
//        Log::log("\n caught unknown exception on Osc::send");
    }
    
    
    return false;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void NetworkController::ProcessMessage( const osc::ReceivedMessage& m, const IpEndpointName& remoteEndpoint )
{
    char ip[IpEndpointName::ADDRESS_STRING_LENGTH ];
    
    remoteEndpoint.AddressAsString( ip );
    

    
    int port = remoteEndpoint.port;

    
    if( !_asyncCallback)
        _delegate->oscReceived( ip, /*remoteEndpoint.*/port , m.AddressPattern(), getVariantListFromOSCMsg( m ) );
    
    else if( !_oscCallbackSelector.empty() && _oscCallbackTarget)
    {
        /*
        EventOscIn* event = new EventOscIn(ip , port , m.AddressPattern() , getVariantListFromOSCMsg( m ) , this);
        Dispatch::getInstance()->pushNotification( event );
         */
        Dispatch::getInstance()->performAsync(this, _oscCallbackTarget, _oscCallbackSelector, {
                                                                                                Variant( ip ) ,
                                                                                                Variant( port ) ,
                                                                                                Variant( m.AddressPattern() ) ,
                                                                                                getVariantListFromOSCMsg( m )
                                                                                                }
                                            );
    }
    

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
 
    NetworkTools namespace
 
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool NetworkTools::changeIpAddressTo(const std::string ip , const std::string netmask , const std::string interface)
{
    struct ifreq ifr;
    const char * name =  interface.c_str();
    
    int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
    
    strncpy(ifr.ifr_name, name, IFNAMSIZ);
    
    ifr.ifr_addr.sa_family = AF_INET;
    
    if ( inet_pton(AF_INET, ip.c_str() , ifr.ifr_addr.sa_data + 2) !=1 )
    {
        printf("\n %s :  unable to set ip to %s " , interface.c_str() ,ip.c_str() );
        return false;
    }
    
    if ( ioctl(fd, SIOCSIFADDR, &ifr) == -1)
    {
        printf("\n error while setting ip address : errno =  %i" , errno );
        return false;        
    }

    
    if ( inet_pton(AF_INET, netmask.c_str(), ifr.ifr_addr.sa_data + 2) != 1)
    {
        printf("\n %s : unable to set netmask to %s " , interface.c_str() ,netmask.c_str() );
        return false;
    }
    
    if ( ioctl(fd, SIOCSIFNETMASK, &ifr) == -1)
    {
        printf("\n error while setting netmask: errno =  %i" , errno );
        return false;        
    }
    
    
    if ( ioctl(fd, SIOCGIFFLAGS, &ifr) == -1)
    {
        printf("\n error : %i" , errno );
        return false;        
    }
    
    
    strncpy(ifr.ifr_name, name, IFNAMSIZ);
    
    ifr.ifr_flags |= (IFF_UP | IFF_RUNNING);
    
    if ( ioctl(fd, SIOCSIFFLAGS, &ifr) == -1)
    {
        printf("\n error : %i" , errno );
        return false;        
    }

    
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

const std::set< std::string > NetworkTools::getInterfacesList()
{
    std::set< std::string> ret;
    
    struct ifaddrs * ifAddrStruct = NULL;
    struct ifaddrs * ifa          = NULL;

    
    getifaddrs( &ifAddrStruct );
    
    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (!ifa->ifa_addr)
            continue;
        
        // check it is IP4 of IPV6
//        if ( 1) //ifa->ifa_addr->sa_family == AF_INET )// || (ifa->ifa_addr->sa_family == AF_INET6) )
        ret.insert( ifa->ifa_name );



    }
    if ( ifAddrStruct != NULL )
        freeifaddrs(ifAddrStruct);
    
    return ret;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

const std::string NetworkTools::getIpAddress( const std::string &/*interfaceName*/ , bool /*ipV6*/)
{
    std::vector<std::string> ips;
    
    FILE *ls = popen("hostname -I", "r");
    char buf[256];
    while (fgets(buf, sizeof(buf), ls) != 0)
    {
        StringOperations::split(buf, ' ', ips);
    }
    pclose(ls);
    
    return ips.at(0);
    /*
    std::string ret = "";
    
    struct ifaddrs * ifAddrStruct = NULL;
    struct ifaddrs * ifa          = NULL;
    void * tmpAddrPtr             = NULL;
    
    getifaddrs( &ifAddrStruct );
    
    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (!ifa->ifa_addr)
        {
            continue;
        }
        
        // check it is IP4
        if ( (ifa->ifa_addr->sa_family == AF_INET) && !ipV6)
        {
            // is a valid IP4 Address
            tmpAddrPtr= &( ( struct sockaddr_in * ) ifa->ifa_addr )->sin_addr;
            
            char addressBuffer[INET_ADDRSTRLEN];
            
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            
            if (strcmp(ifa->ifa_name, interfaceName.c_str() ) == 0)
            {
                ret = addressBuffer ;
            }

        }
        
        // check it is IP6
        else if ( ( ifa->ifa_addr->sa_family == AF_INET6) && ipV6)
        {
            // is a valid IP6 Address
            tmpAddrPtr= &( ( struct sockaddr_in6 * ) ifa->ifa_addr )->sin6_addr;
            
            char addressBuffer[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
            
            if (strcmp(ifa->ifa_name, interfaceName.c_str() ) == 0)
            {
                ret = addressBuffer ;
            }
            
        }
    }
    if ( ifAddrStruct != NULL )
        freeifaddrs(ifAddrStruct);
    
    return ret;
     */
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

const std::string NetworkTools::getHostName()
{
    /*
    std::string ret;
    
    struct addrinfo hints;
    struct addrinfo *info = nullptr;
    struct addrinfo *p    = nullptr;
    
    int result = -1;
    */
    char hostname[1024];
    
    hostname[1023] = '\0';
    gethostname(hostname, 1023);
    
    return hostname;
    /*
    memset(&hints, 0, sizeof hints);
    
    hints.ai_family = AF_UNSPEC; //either IPV4 or IPV6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;
    
    if (( result = getaddrinfo(hostname, "http", &hints, &info)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror( result ) );

        return std::string();
    }
    
    for(p = info; p != NULL; p = p->ai_next)
    {
        printf("hostname: %s\n", p->ai_canonname);
    }
    
    freeaddrinfo(info);
    
    return ret;
    */
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool NetworkTools::isConnected()
{

    printf("\n NetworkTools::isConnected() not implemented!");

    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

#ifdef ENABLE_ELEMENT_SELECTOR
const Variant NetworkController::performSelectorWithArguments( const std::string &selector , const VariantList  &arguments , bool *validCall)
{
    if ( validCall )
        *validCall = true;
    
    if( selector == "addPort")
    {
        return Variant ( addPort( arguments.at(0).getInt() ) );
    }
    
    else if ( selector == "removePort")
    {
        return Variant ( removePort( arguments.at(0).getInt() ) );
    }
    
    else if ( selector == "sendOSC")
    {

        return Variant ( sendOSC( arguments.at(0).getString(),
                                  arguments.at(1).getInt(),
                                  arguments.at(2).getString() ,
                                  arguments.at(3).getList()
                                )
                        );
    }
    else if ( selector == "sendOSCBroadcast")
    {
        
        return Variant ( sendOSC( arguments.at(0).getString(),
                                 arguments.at(1).getInt(),
                                 arguments.at(2).getString() ,
                                 arguments.at(3).getList(),
                                 true
                                 )
                        );
    }
    
    else if( selector == "setOscCallback")
    {
        _oscCallbackSelector = arguments.at(0).getString();
        _oscCallbackTarget   = Element::getElementById( arguments.at(1).getUInt64() );
        
        return Variant( _oscCallbackTarget != nullptr );
    }
    
    else if ( selector == "getIp")
        return Variant(NetworkTools::getIpAddress( arguments.at(0).getString() ));

    else if (selector == "getHostName")
        return Variant( NetworkTools::getHostName() );
    
    else if (selector == "stop")
        return Variant(stop() );
    
    if ( validCall )
        *validCall = false;
    
    return Element::performSelectorWithArguments(selector, arguments , validCall );
}
#endif



