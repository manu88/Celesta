//
//  Utils.h
//  Broadway_core
//
//  Created by Manuel Deneu on 30/03/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_core__Utils__
#define __Broadway_core__Utils__

#include <iostream>
#include <tuple>

namespace Utils
{
    bool checkLittleEndian();
    bool checkBigEndian();

}

namespace System
{
    float getTemp();
    int getGpuMem();
    
    int getPid();
    
    std::tuple<float , float > getCpuAndMemPercent();
    
    unsigned long getTimeSinceStart();

}

#endif /* defined(__Broadway_core__Utils__) */
