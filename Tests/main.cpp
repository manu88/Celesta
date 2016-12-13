//
//  main.cpp
//  Celesta
//
//  Created by Manuel Deneu on 13/12/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//

#include <stdio.h>
#include <assert.h>

#include "../Core/Core.h"
#include "VariantUnitTest.hpp"
#include "PListUnitTest.hpp"
#include "DictionaryUnitTest.hpp"

int main(int argc, const char * argv[])
{
    
    printf("Starting Celesta unit Tests\n");
    
    {
        VariantUnitTest t0;
        t0.run();
        
        PListUnitTest t1;
        t1.run();
        
        DictionaryUnitTest t2;
        t2.run();
        
    }
    
    
    
    assert( GBObjectIntrospection(1) == 0);
    
}
