//
//  SerialInterface.h
//  Broadway_core
//
//  Created by Manuel Deneu on 15/12/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_core__SerialInterface__
#define __Broadway_core__SerialInterface__

#include <termios.h> // for speed_t def
#include <iostream>
#include "InterfaceEvent.h"

typedef enum
{
    Serial_0      = 0, // unspecified
    Serial_1200   = 1,
    Serial_2400   = 2,
    Serial_4800   = 3,
    Serial_9600   = 4,
    Serial_19200  = 5,
    Serial_38400  = 6,
    Serial_57600  = 7,
    Serial_115200 = 8,
    Serial_250000 = 9
    
} SerialSpeed;

class SerialEvent : public InterfaceEvent , public Element
{
public:
    
    static const std::vector<std::string> getSerialDevicesList();

    SerialEvent();
    SerialEvent( const std::string port );
    
    virtual ~SerialEvent();

    bool setSpeed ( const SerialSpeed speed );
    
    SerialSpeed getSpeed() const
    {
        return _speed;
    }
    
    const std::string getPort() const
    {
        return _port;
    }
    
    void setPort( const std::string &port )
    {
        _port = port;
    }
    
    bool isOpen() const
    {
        return _isOpen;
    }
    
    void clearBuffer();
    
    bool changed();
    const BytesList readBytes() const;
    const std::string readString() const;
    
    int  read( void *buff) const;
    
    inline ssize_t getBytesToRead() const noexcept
    {
        return _bytesToRead;
    }
    
    void setBytesToRead( const ssize_t &num ) noexcept
    {
        _bytesToRead = num;
    }
    
    inline ssize_t getAvailable() const noexcept
    {
        return _numBytes;
    }
    

    bool send( const uint8_t* datas, const unsigned int len) const;
    bool send( const char* datas) const;

    bool openPort();
    bool closePort();
    
#ifdef ENABLE_ELEMENT_SELECTOR
    const Variant performSelectorWithArguments( const std::string &selector , const VariantList  &arguments, bool *validCall = nullptr ) ;
#endif
    
protected:
    
    void cleanup()
    {
        closePort();
    }

    static speed_t getInternalSpeed( const SerialSpeed speed );
    static SerialSpeed getSerialSpeed( speed_t internalSpeed );
    
private:
    
    bool setCustomSpeed( speed_t rate);
    bool          _isOpen;
    std::string   _port;
    int           _fd;
    
    ssize_t _bytesToRead;
    
    mutable ssize_t   _numBytes;
    
    bool _hasChanged;
    
    SerialSpeed _speed;
    
};

#endif /* defined(__Broadway_core__SerialInterface__) */
