//
//  Event.cpp
//  
//
//  Created by Manuel Deneu on 12/08/14.
//
//

#include "Event.h"
#include "../Env_Variables.h"

int EventSelector::s_count = 0;
int EventMessage::s_count = 0;


Event::Event( EventType type , Element* sender ) :
_type( type ),
_sender( sender)

{

    //setElementName( ClassNames::Event );
    className = ClassNames::Event;
}

Event::~Event()
{
    
}
/*
#ifdef ENABLE_ELEMENT_SELECTOR
const Variant Event::performSelectorWithArguments( const std::string &selector , const VariantList  &arguments, bool *validCall  )
{
    return Element::performSelectorWithArguments(selector, arguments, validCall);
}

#endif
*/
