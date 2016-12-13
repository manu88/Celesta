//
//  Config.h
//  Broadway_test
//
//  Created by Manuel Deneu on 21/10/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef Broadway_test_Config_h
#define Broadway_test_Config_h

extern "C"
{
    #include "../../FlyLab/GroundBase/include/GroundBase.h"
}




struct ScopedGBRef
{
    ScopedGBRef( GBRef val):
    value(val)
    {}
    
    ~ScopedGBRef()
    {
        GBRelease( value);
    }
    
    GBRef value;
    
};


#endif
