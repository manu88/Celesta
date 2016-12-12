//
//  InterfaceEvent.cpp
//  Broadway_core
//
//  Created by Manuel Deneu on 15/12/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#include "InterfaceEvent.h"




InterfaceEvent::InterfaceEvent( InterfaceEventType type , Element *sender ) :
Event ( Event_Input , sender ),
_type ( type )
{
    
}

InterfaceEvent::~InterfaceEvent()
{
    
}

