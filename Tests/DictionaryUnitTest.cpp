//
//  DictionaryUnitTest.cpp
//  Celesta
//
//  Created by Manuel Deneu on 13/12/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//

#include "DictionaryUnitTest.hpp"


DictionaryUnitTest::DictionaryUnitTest(): UnitTestBase("Dictionary")
{
    
}

bool DictionaryUnitTest::test()
{
    Dictionary dictCpy;
    
    {
        Dictionary dict;
        
        assert( dict.contains("Hello") == false);
        
        assert(dict.addValueForKey(Variant("Hello test value"), "key1") );
        assert( dict.contains("key1") );
        
        const Variant v1 = dict.getValueForKey("key1");
        
        assert(v1.isNull() == false);
        
        printf("v1 value '%s' \n" , v1.getString().c_str() );
        
        dictCpy = dict;
    }
    
    const Variant v1 = dictCpy.getValueForKey("key1");
    
    assert(v1.isNull() == false);
    
    printf("v1 value '%s' \n" , v1.getString().c_str() );
    return true;
}
