//
//  Peripherals.hpp
//  JSCenter
//
//  Created by Manuel Deneu on 03/11/2015.
//  Copyright © 2015 Manuel Deneu. All rights reserved.
//

#ifndef Peripherals_hpp
#define Peripherals_hpp

#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <vector>


#ifndef __APPLE__
#include <linux/input.h>
#else
struct input_event {};
#endif

#include "../GXDataType/GXGeometry.h"
typedef struct
{
    GXPoint center;
    int     id;
    int     touchState;
} GXTouch;


class TouchEvent
{
public:
    
    enum { MaxTouches = 10 };
    enum { NoTouchTimeOut = 100 }; // MS
    TouchEvent();
    ~TouchEvent();


    /*
      -1 error
      0 no read
      > 0 ok
     */
    int  readTouches();

    const std::string &getDeviceName() const noexcept
    {
        return _deviceName;
    }
    
    const std::vector<GXTouch> &getTouchesList() const
    {
        return _touchesList;
    }
    
    inline const GXSize &getSize() const
    {
        return _size;
    }
private:
    
    void cleanup();
    
    std::string _deviceName;

    struct input_event ev;
    int fd;
    
    std::vector<GXTouch> _touchesList;
    GXSize _size;

};

#endif /* Peripherals_hpp */
