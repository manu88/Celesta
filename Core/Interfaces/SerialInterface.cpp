//
//  SerialInterface.cpp
//  Broadway_core
//
//  Created by Manuel Deneu on 15/12/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/ioctl.h>

#include "SerialInterface.h"

#include "../Internal/FileSystem.h"
#include "../Log/Log.h"

SerialEvent::SerialEvent() :
InterfaceEvent( Event_Serial  , this ),

_isOpen      ( false ),
_port        ( ""  ),
_fd          ( -1    ),
_bytesToRead ( 1     ),
_numBytes    ( 0     ),
_hasChanged  ( false ),

_speed      ( Serial_0 )
{
    setElementName("SerialEvent");
    className ="SerialEvent";
}

SerialEvent::SerialEvent( const std::string port ) :
::SerialEvent()
{
    _port = port;
}

SerialEvent::~SerialEvent()
{
    closePort();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool SerialEvent::openPort()
{
    if ( _isOpen )
        return true;

    _fd = open( _port.c_str(),O_RDWR | O_NOCTTY | O_NDELAY);// O_RDWR | O_NONBLOCK);
    
    if ( _fd == -1)
        Log::log(" ERROR : unable to open port '%s' ." , _port.c_str() );


    _isOpen = ( _fd != -1 );
    
    setSpeed( Serial_9600 );
    
    return _isOpen;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool SerialEvent::closePort()
{
    if (_isOpen)
    {
        int ret = close( _fd );
        
        if ( ret != 0 )
            return false;
        
        _isOpen = false;
            
    }
    
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool SerialEvent::setSpeed ( const SerialSpeed speed )
{
    if ( speed != _speed)
    {
        _speed = speed;
        
        speed_t spd =getInternalSpeed( _speed );
        
        if (spd == 0)
            setCustomSpeed( spd );
        
        struct termios options;

        if( tcgetattr( _fd, &options ) <0 )
        {
            Log::log("Couldn't get term attributes");
            return false;
        }
        
        cfsetispeed( &options,  spd?:B38400 );
        cfsetospeed( &options,  spd?:B38400 );
        
        options.c_cflag = CS8|CREAD|CLOCAL;// |= (CLOCAL | CREAD);

        options.c_iflag=0;
        options.c_oflag=0;
        
        options.c_lflag=0;
        options.c_cc[VMIN]=1;
        options.c_cc[VTIME]=5;

        const int ret =  tcsetattr( _fd, TCSANOW, &options);
        

        
        if ( getSerialSpeed( cfgetispeed( &options ) ) !=  _speed )
        {
            printf("Error ispeed\n");
        }
        
        if ( getSerialSpeed( cfgetospeed( &options ) ) != _speed )
        {
            printf("Error ospeed\n");
        }

        
        if ( ret == -1 )
        {
            return false;
        }

    }
    
    return true;
}

bool SerialEvent::setCustomSpeed(speed_t /*rate*/)
{
    /*
    struct serial_struct serinfo;
    

    serinfo.reserved_char[0] = 0;
    if (ioctl(fd, TIOCGSERIAL, &serinfo) < 0)
        return -1;
    serinfo.flags &= ~ASYNC_SPD_MASK;
    serinfo.flags |= ASYNC_SPD_CUST;
    serinfo.custom_divisor = (serinfo.baud_base + (rate / 2)) / rate;
    if (serinfo.custom_divisor < 1)
        serinfo.custom_divisor = 1;
    if (ioctl(fd, TIOCSSERIAL, &serinfo) < 0)
        return -1;
    if (ioctl(fd, TIOCGSERIAL, &serinfo) < 0)
        return -1;
    if (serinfo.custom_divisor * rate != serinfo.baud_base) {
        warnx("actual baudrate is %d / %d = %f",
              serinfo.baud_base, serinfo.custom_divisor,
              (float)serinfo.baud_base / serinfo.custom_divisor);
    }
     */
    return false;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ speed_t SerialEvent::getInternalSpeed( const SerialSpeed speed )
{
    switch (speed)
    {
        case Serial_1200:
            return B1200;
        case Serial_2400:
            return B2400;
        case Serial_4800:
            return B4800;
        case Serial_9600:
            return B9600;
        case Serial_19200:
            return B19200;
        case Serial_38400:
            return B38400;
        case Serial_57600:
            return B57600;
        case Serial_115200:
            return B115200;
            /* Other cases will return 0 */



            
        default:
            break;
    }
    
    return 0; // default
}

/*static*/ SerialSpeed SerialEvent::getSerialSpeed( speed_t internalSpeed )
{
    switch ( internalSpeed )
    {
        case B1200:
            return Serial_1200;
        case B2400:
            return Serial_2400;
        case B4800:
            return Serial_4800;
        case B9600:
            return Serial_9600;
        case B19200:
            return Serial_19200;
        case B38400:
            return Serial_38400;
        case B57600:
            return Serial_57600;
        case B115200:
            return Serial_115200;
        case 250000:
            return Serial_250000;
            
            
        default:
            break;
    }
    
    return Serial_0;
}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
void SerialEvent::clearBuffer()
{
    tcflush(_fd,TCIOFLUSH);
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool SerialEvent::changed()
{
    ioctl( _fd, FIONREAD, &_numBytes);

    return _hasChanged = ( _numBytes  >= _bytesToRead );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/* Base method to be called. Following functions ( readBytes , readString, etc.. call it */
int  SerialEvent::read( void *buff) const
{
    if ( _numBytes > 0)
    {

        const ssize_t num = ::read( _fd, buff, (size_t) _numBytes );

        if( num == -1)
        {
            printf("\n ERROR READ : %i", errno );
            return -1;
        }
        
        _numBytes -= num;
        
        return static_cast<int>( num );
    }
    
    return 0;
}

/* **** **** **** **** **** **** **** **** **** */

const BytesList SerialEvent::readBytes() const
{
    uint8_t in[ getAvailable() ];
    
    const int r = read( in );

    if(  r > 0 )
    {
        BytesList l;
        
        for (int i=0;i< r ;i++)
            l.push_back( in[i]);

        return l;
    }
    
    return BytesList();
}

/* **** **** **** **** **** **** **** **** **** */

const std::string SerialEvent::readString() const
{
    char in[ getAvailable()+1 ];
    
    const int r = read( in );
    
    if(  r > 0 )
    {
        in[r] = 0;
        
        return std::string( in );
    }
    
    return std::string();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
bool SerialEvent::send( const uint8_t* datas , const unsigned int len) const
{
    if ( !_isOpen )
        return false;
    
    int numWritten = ( int ) ::write( _fd, datas ,len );
    
    if (numWritten == -1)
        printf("Error write() %s\n", strerror(errno));
    
    return numWritten > 0;
}

bool SerialEvent::send( const char* datas) const
{
    if ( !_isOpen )
        return false;
    
    int numWritten = ( int ) ::write( _fd, datas ,strlen( datas ) );
    
    if (numWritten == -1)
        printf("Error write() %s\n", strerror(errno));

    return numWritten > 0;
    
}

/*
bool SerialEvent::send( int val)
{
    char buf[4];
    sprintf(buf, "%i" , val);

    return send( buf );
    
}
*/
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ const std::vector<std::string> SerialEvent::getSerialDevicesList()
{
#ifdef __APPLE__
    
    auto list =  FileSystem::getFilesListFromFolder("/dev/",true ,true,"tty."  );
    
#elif defined __unix__

    auto list =  FileSystem::getFilesListFromFolder("/dev/",true, true ,"tty" );
    
#endif
    
    return list;
}

#ifdef ENABLE_ELEMENT_SELECTOR
const Variant SerialEvent::performSelectorWithArguments( const std::string &selector , const VariantList  &arguments, bool *validCall )
{
    if( validCall)
        *validCall = true;
    
    if( selector == "setPort")
    {
        setPort( arguments.at(0).getString() );
        return  Variant::null();
    }
    
    else if ( selector == "getPort")
    {
        return Variant( getPort() );
    }
    
    else if ( selector == "setSpeed")
    {
        return Variant(setSpeed( (SerialSpeed) arguments.at(0).getInt() ) );
    }
    
    else if (selector == "send")
    {
        return Variant( send( arguments.at(0).getString().c_str() ));
        
    }
    
    else if (selector == "getSerialDevicesList")
    {
        VariantList ret;
        
        for( const auto &p : getSerialDevicesList())
            ret.push_back( Variant(p) );
        
        return Variant( getSerialDevicesList() ) ;
        
    }
    
    else if ( selector == "setBytesToRead")
    {
        setBytesToRead( arguments.at(0).getInt() );
        return Variant::null();
    }
    
    else if ( selector == "getBytesToRead")
    {

        return Variant( (Variant::CastUIntAs) getBytesToRead() );
    }
    
    else if ( selector == "read")
    {
        assert(false);
        return readBytes();
    }
    
    else if( selector == "isOpen")
    {
        return Variant( isOpen() );
    }
    
    if( validCall)
        *validCall = false;
    
    return Element::performSelectorWithArguments(selector, arguments, validCall);
}
#endif
