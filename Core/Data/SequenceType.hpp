//
//  SequenceType.hpp
//  Celesta
//
//  Created by Manuel Deneu on 13/12/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//

#ifndef SequenceType_h
#define SequenceType_h

#include "Variant.h"

class SequenceType
{
public:
    
    // required
    virtual bool addValueForKey(const Variant &value , const std::string &key) noexcept = 0;
    virtual Variant getValueForKey( const std::string &key) const noexcept = 0;
    virtual bool contains( const std::string &key) const noexcept = 0;
    
    virtual bool remove(const std::string &key) noexcept = 0;
    
};

#endif /* SequenceType_h */
