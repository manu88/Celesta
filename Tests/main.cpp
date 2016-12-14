//
//  main.cpp
//  Celesta
//
//  Created by Manuel Deneu on 13/12/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//

#include <stdio.h>
#include <assert.h>
#include <vector>
#include "../Core/Core.h"
#include "VariantUnitTest.hpp"
#include "PListUnitTest.hpp"
#include "DictionaryUnitTest.hpp"
#include "TimerUnitTest.hpp"


int main(int argc, const char * argv[])
{
    Log::addLocalLogger();
    
    printf("Starting Celesta unit Tests\n");
    
    {
        std::vector<UnitTestBase*> tests;
        
        tests.push_back( new VariantUnitTest() );
        tests.push_back( new DictionaryUnitTest());
        tests.push_back( new PListUnitTest() );
        tests.push_back( new TimerUnitTest());
        
        for( UnitTestBase* test : tests)
        {
            test->run();
            delete test;
        }
        
    }
    
    
    
    assert( GBObjectIntrospection(1) == 0);
    
}
