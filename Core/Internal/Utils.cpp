//
//  Utils.cpp
//  Broadway_core
//
//  Created by Manuel Deneu on 30/03/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//
#include <sys/types.h>
#include <unistd.h>
#include "Utils.h"

#ifdef TARGET_RASPBERRY_PI
#include <bcm_host.h>
#endif


bool Utils::checkLittleEndian()
{
    const unsigned int lilEndianTest = 1;

    return (((unsigned char *)&lilEndianTest)[0] == 1);
}

bool Utils::checkBigEndian()
{
    return !checkLittleEndian();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

float System::getTemp()
{
    FILE *temperature;
    float t = -1;
    temperature = fopen ("/sys/class/thermal/thermal_zone0/temp", "r");
    if (temperature != NULL)
    {
        int tt = 0;
        fscanf (temperature, "%i", &tt);
        t =  tt/1000.0f;
        fclose ( temperature );
    }
    
    return t;
}

int System::getGpuMem()
{

#ifdef TARGET_RASPBERRY_PI
    char response[80] = "";
    int gpu_mem = 0;
    if ( vc_gencmd(response, sizeof response, "get_mem gpu") == 0 )
        vc_gencmd_number_property(response, "gpu", &gpu_mem);
    return gpu_mem;
#endif

    return -1;
}



int System::getPid()
{
    return ::getpid();
}

std::tuple<float , float > System::getCpuAndMemPercent()
{
    float cpu = -1.0f;
    float mem = -1.0f;
    const std::string call = "ps -p "+ std::to_string( getPid() )+ " -o %cpu,%mem | sed '/%CPU/d'";
    FILE *fp = popen( call.c_str() , "r");
    
    fscanf(fp, "\n");
    fscanf(fp, "%f %f", &cpu , &mem);
    pclose(fp);

    
    return std::make_tuple(cpu , mem);
}

unsigned long System::getTimeSinceStart()
{
    
    //ls -ld /proc/3189
//    dr-xr-xr-x 7 root root 0 Jul 15 14:51
    unsigned long ret = 0;
#ifdef __APPLE__
    // no procfs
    
#else
    
    const std::string call = "ls -ld /proc/"+ std::to_string( getPid() );
    FILE *fp = popen( call.c_str() , "r");

    printf("\n Call : '%s'" , call.c_str() );
    if( !fp)
    {
        printf("\n Error '%s'" , call.c_str());
        return -1;
    }
    int hh = -1;
    int min  = -1;
    char dump[64];
    int  d =-1;
    fscanf( fp ,"%s %i %s %s %i %s %i %i:%i %s" , dump,&d,dump,dump,&d,dump,&d,&hh,&min,dump );

    return hh*60 + min;
    
    pclose(fp);
#endif
    return ret;
}


