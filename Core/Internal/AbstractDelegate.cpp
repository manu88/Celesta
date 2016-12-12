//
//  AbstractDelegate.cpp
//  MediaCenter
//
//  Created by Manuel Deneu on 04/04/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#include "AbstractDelegate.h"
#include <assert.h>


AbstractDelegate::AbstractDelegate() noexcept
{
}


AbstractDelegate::~AbstractDelegate()
{    
}


bool AbstractDelegate::delegateReadyForController( const AbstractController*  )
{
    return true;
}
