//
//  SerializableObject.h
//  JSCenter
//
//  Created by Manuel Deneu on 29/11/2015.
//  Copyright © 2015 Manuel Deneu. All rights reserved.
//

#ifndef SerializableObject_h
#define SerializableObject_h

#ifdef USE_JSON_PARSER    
#include "../Parsers/JSONParser.h"
#endif

class SerializableObject
{
protected:
    SerializableObject() {}
    virtual ~SerializableObject() {}
    
public:
    
#ifdef USE_JSON_PARSER
    virtual bool parseJSON( const JSON::Node &node) = 0;
    virtual bool saveJSON( JSON::Node &node) const = 0 ;
#endif
    
};

#endif /* SerializableObject_h */
