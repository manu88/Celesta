//
//  AUElement.cpp
//  
//
//  Created by Manuel Deneu on 16/08/14.
//
//

#include "AUElement.h"


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */


AUElement::AUElement()
{
    className = "AUElement";
}

AUElement::~AUElement()
{
    
}

#ifdef ENABLE_ELEMENT_SELECTOR

const Variant performSelectorWithArguments( const std::string &selector , const VariantList  &arguments, bool *validCall = nullptr ) ;
#endif

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
