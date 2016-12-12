//
//  MainPlayer.cpp
//  Broadway_test
//
//  Created by Manuel Deneu on 05/10/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#include "MainPlayer.h"
#include "../../../GXExtra/GXVideo.h"

void MainPlayer::run()
{
    _parent->sig_ready();
    
    while (!threadShouldStop())
    {
        sleepForMs(300);
    }
}
