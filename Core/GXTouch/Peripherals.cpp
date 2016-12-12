//
//  Peripherals.cpp
//  JSCenter
//
//  Created by Manuel Deneu on 03/11/2015.
//  Copyright © 2015 Manuel Deneu. All rights reserved.
//


#include "Peripherals.h"
#include "../Core.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>

#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1)/BITS_PER_LONG)+1)
#define OFF(x)  ((x)%BITS_PER_LONG)
#define test_bit(bit, array)    ((array[LONG(bit)] >> OFF(bit)) & 1)


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

TouchEvent::TouchEvent():
_size( makeSizeNULL() )
{
    char name[256] = "Unknown";
    
    fd = open("/dev/input/event0", O_RDONLY);
    if (fd == -1)
    {
        printf(" unable to open device\n");
        return;
    }
#ifndef __APPLE__
    
    int have_multitouch= 0;
    int have_track_id = 0;
    double xMin = 0;
    double yMin = 0;
    double xMax = -1;
    double yMax = -1;
    
    ioctl(fd, EVIOCGNAME(sizeof(name)), name);

    /**/
    unsigned long bits[NBITS(KEY_MAX)];
    struct input_absinfo abs;
    
    ioctl (fd, EVIOCGBIT (EV_ABS, KEY_MAX), bits);
    if (!(test_bit (ABS_MT_POSITION_X, bits) &&
          test_bit (ABS_MT_POSITION_Y, bits)))
    {
        ioctl (fd, EVIOCGABS (ABS_X), &abs);
        xMin  = abs.minimum;
        xMax  = abs.maximum;
        ioctl (fd, EVIOCGABS (ABS_Y), &abs);
        yMax = abs.maximum;
        have_multitouch = 0;
        //context->min.id = -1;
        //context->max.id = -1;
    }
    else
    {
        ioctl (fd, EVIOCGABS (ABS_MT_POSITION_X), &abs);
        xMin = abs.minimum;
        xMax = abs.maximum;
        ioctl (fd, EVIOCGABS (ABS_MT_POSITION_Y), &abs);
        yMin = abs.minimum;
        yMax = abs.maximum;
        have_multitouch = 1;

    }
    
    have_track_id = test_bit (ABS_MT_TRACKING_ID, bits) ? 1 : 0;
/*

    printf ( "   coordinate range X: %f - %f, Y: %f - %f\n",
            xMin, xMax,
            yMin, yMax);
  */  
    if( (xMin != -1) && (xMax != -1) && (yMin != -1) && (yMax != -1))
        _size = makeSize( xMax - xMin , yMax - yMin );
    /**/
#endif


    _deviceName = name;

}

TouchEvent::~TouchEvent()
{
    close( fd );
}



int TouchEvent::readTouches()
{
    
    int ret = 1; // moved
    
    _touchesList.clear();
    
#ifndef __APPLE__

    int i;

    size_t rb;

    struct input_event ev[64];
    
    fd_set read_fds;
    fd_set write_fds;
    fd_set except_fds;
    
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    FD_ZERO(&except_fds);
    FD_SET(fd, &read_fds);
    
    // Set timeout to 1.0 seconds
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 1000 * NoTouchTimeOut;
    
    if (select(fd + 1, &read_fds, &write_fds, &except_fds, &timeout) != 1)
    {
        return -1;
    }

    rb=read(fd,ev,sizeof(struct input_event)*64);

    const int count =rb / sizeof(struct input_event);
    
    if( rb < 0)
        return -1;
    
    else if( rb == 0 )
        return 0;

    
    int touchNum = 0;
    int x = -1;
    int y = -1;

    int touchState = 0;
    
    for (i = 0;  i <  count; i++)
    {
        if( ev[i].type == EV_ABS && ev[i].code == 47) //ABS_MT_SLOT
        {
            touchNum =ev[i].value;
        }
        else if (ev[i].type == EV_ABS  && ev[i].code == 53 /*ABS_MT_POSITION_X*/ && ev[i].value > 0)
        {
            x =ev[i].value;
            
            if( touchState == 0)
                touchState = 2;
            
            _touchesList.push_back({ makePoint(x, -1) , touchNum , touchState} );

        }
        else if (ev[i].type == EV_ABS  && ev[i].code == 54 /*ABS_MT_POSITION_Y*/ && ev[i].value > 0)
        {
            y =  _size.height - ev[i].value ;
            if( touchState == 0)
                touchState = 2;
            
            _touchesList.push_back({ makePoint(-1, y) , touchNum , touchState} );
        }
        else if (ev[i].type == EV_ABS  && ev[i].code == 57 /*ABS_MT_TRACKING_ID*/ )
        {
            if( ev[i].value == -1 ) // track ended
            {
                touchState = 3;
                _touchesList.push_back({ makePoint(-1, -1) , touchNum , touchState} );
            }
            else // track began
            {
                touchState = 1;
                _touchesList.push_back({ makePoint(-1, -1) , touchNum , touchState} );
            }
        }
    }
#else
    Thread::sleepForMs(30);
#endif

    return ret;
}
void TouchEvent::cleanup()
{
    
}

