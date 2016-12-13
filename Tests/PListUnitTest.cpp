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
    {
        printf("PList Write \n");
        PList::Document document;
        
        if( document.addValueForKey(Variant(12), "key1") == false)
        {
            printf("Unable to add value 12 \n");
            return  false;
        }
        
        if( document.addValueForKey(Variant("test String bla bla"), "key2") == false)
        {
            printf("Unable to add value string \n");
            return  false;
        }
        const Variant v = document.getValueForKey("key1");
        
        if( v.isNull())
        {
            printf("v is NULL\n");
            return false;
        }
        
        printf("value %i \n" , v.getInt());

        const Variant vFake = document.getValueForKey("lolz");
        
        assert(vFake.isNull());
        
        document.save("test.plist");
    }
    
    {
        printf("PList Read \n");
        PList::Document outDoc("test.plist");
        
        const Variant vKey1 = outDoc.getValueForKey("key1");
        
        if( vKey1.isNull())
        {
            printf("vKey1 is NULL\n");
            return false;
        }
        
        printf("value %i \n" , vKey1.getInt());
        
        const Variant vKey2 = outDoc.getValueForKey("key2");
        
        if( vKey2.isNull())
        {
            printf("vKey2 is NULL\n");
            return false;
        }
        
        printf("value %s \n" , vKey2.getString().c_str() );

                               
    }
    return false;
}
