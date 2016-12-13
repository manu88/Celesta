//
//  PListUnitTest.cpp
//  Celesta
//
//  Created by Manuel Deneu on 13/12/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//

#include "PListUnitTest.hpp"


PListUnitTest::PListUnitTest():
UnitTestBase("Plist")
{
    
}

bool PListUnitTest::test()
{
    PList::Document document;
    
    if( document.addValueForKey(Variant(12), "key1") == false)
    {
        printf("Unable to add value \n");
        return  false;
    }
    const Variant v = document.getValueForKey("key1");
    
    if( v.isNull())
    {
        printf("v is NULL\n");
        return false;
    }
    
    printf("value %i \n" , v.getInt());
    
    
    document.save("test.plist");
    return false;
}
