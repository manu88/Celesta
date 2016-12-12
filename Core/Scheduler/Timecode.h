//
//  TimeCode.h
//  VirtualMachine
//
//  Created by Manuel Deneu on 26/06/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef VirtualMachine_TimeCode_h
#define VirtualMachine_TimeCode_h

#include <string>
#include <assert.h>

#include <sys/time.h>

#include <sstream>

#include "../Data/StringOperations.h"

#include "../Data/Variant.h"


struct Timecode_t
{
    unsigned long h;
    unsigned long min;
    unsigned long sec;
    unsigned long milli;
    
public:
    
    static Timecode_t getCurrent()
    {
        Timecode_t t;
        t.setToCurrentTime();
        
        return t;
    }

    Timecode_t ( unsigned long heures = 0 , unsigned long minutes =0 , unsigned long secondes = 0 , unsigned long milliSecs = 0 ) :
        h    ( heures    ),
        min  ( minutes   ),
        sec  ( secondes  ),
        milli( milliSecs )
    {
        reconstruct();
    }
    
    Timecode_t ( const Variant &var):
    ::Timecode_t( var.getList().at(0).getULong() ,
                  var.getList().at(1).getULong() ,
                  var.getList().at(2).getULong() ,
                  var.getList().at(3).getULong()
                 )
    {}
    
    Variant getVariant() const
    {
        return Variant( {
                            Variant( h     ),
                            Variant( min   ),
                            Variant( sec   ),
                            Variant( milli ),
                      } );
    }
    
    Timecode_t ( const std::string &string /*separated by ':' -> HH:MM:SS:MSS */ ) :
    h    ( 0 ),
    min  ( 0 ),
    sec  ( 0 ),
    milli( 0 )
    {
        auto list = StringOperations::split( string , ':' );

        if ( list.size() >= 1)
            h     = static_cast<unsigned long>( atoi( list.at(0).c_str() ) );
        
        if ( list.size() >= 2)
            min   = static_cast<unsigned long>( atoi( list.at(1).c_str() ) );
        
        if ( list.size() >= 3)
            sec   = static_cast<unsigned long>( atoi( list.at(2).c_str() ) );
        
        if ( list.size() >= 4)
            milli = static_cast<unsigned long>( atoi( list.at(3).c_str() ) );

        reconstruct();
    }
    
    void setToCurrentTime()
    {
        time_t rawtime;
        struct tm * timeinfo;
        
        time (&rawtime);
        timeinfo = localtime (&rawtime);
        
        h     = static_cast<unsigned long>( timeinfo->tm_hour );
        min   = static_cast<unsigned long>( timeinfo->tm_min  );
        sec   = static_cast<unsigned long>( timeinfo->tm_sec  );
        milli = 0;
        /*
        
        struct timeval t;
        
        gettimeofday( &t, 0 );
        
        h = 0;
        min = 0;
        sec = 0;
        milli = ((double)t.tv_sec*1000.);// + ((double)t.tv_usec / 1000.);
        
        reconstruct();
         */
    }

    
    
    void reconstruct()
    {
        auto add = milli / 1000.;
        milli %= 1000;
        
        sec+=add;
        
        add = sec / 60. ;
        sec %= 60;
        
        min+=add;
        
        add = min / 60. ;
        min %= 60;
        
        h += add;
        
        h %= 24;
    }
    
    void addMillis( unsigned long millis)
    {
        milli += millis;
        reconstruct();
    }
    
    unsigned long getInMs() const
    {
        return milli + 1000*(sec + 60*(min +60*h));
        
    }
    
    bool isNull() const
    {
        return getInMs() == 0 ;
    }
    
    bool isValid() const
    {
        return !isNull();
    }
    
    std::string getString(bool displayH = true , bool displayMin = true , bool displaySec = true , bool displayMilli = true ) const
    {

        std::stringstream stream;
        if ( displayH )
            stream << StringOperations::Formatter('0' , 2) << h     << ":";
        
        if ( displayMin )
            stream << StringOperations::Formatter('0' , 2) << min   << ":";
        
        if ( displaySec )
            stream << StringOperations::Formatter('0' , 2) << sec   ;
        
        if ( displayMilli )
            stream << "::"<< StringOperations::Formatter('0' , 3) << milli ;
        
        return stream.str();
    }
    

    
    
};



typedef struct Timecode_t Timecode;

// invalid Time Code
static Timecode InvalidTimeCode( 2000 , 2000 , 2000 ,  2000 );

bool operator==(const Timecode& lhs, const Timecode& rhs);
bool operator<(const Timecode& lhs, const Timecode& rhs);
bool operator>(const Timecode& lhs, const Timecode& rhs);
bool operator!=(const Timecode& lhs, const Timecode& rhs);
bool operator>=(const Timecode& lhs, const Timecode& rhs);
bool operator<=(const Timecode& lhs, const Timecode& rhs);

Timecode operator+=(Timecode& lhs, const Timecode& rhs);
Timecode operator-=(Timecode& lhs, const Timecode& rhs);

Timecode operator-(const Timecode& lhs, const Timecode& rhs);

Timecode operator+(const Timecode& lhs, const Timecode& rhs);
void printTime(Timecode tc);
unsigned long getTimeToWaitInMS(Timecode &currentTime , Timecode &tc );
bool isValidTimecode(Timecode &tc);

#endif
