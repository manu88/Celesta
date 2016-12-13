//
//  VariantUnitTest.cpp
//  Celesta
//
//  Created by Manuel Deneu on 13/12/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//


#include <string>
#include "VariantUnitTest.hpp"


VariantUnitTest::VariantUnitTest():
UnitTestBase("Variant base test")
{
    
}


bool VariantUnitTest::test()
{
    const char* str = "Hello";
    Variant strVal(str);
    
    assert( strVal.isString());
    assert( strcmp(str, strVal.getString().c_str()) == 0);
    printf("strVal value '%s'\n" , strVal.getString().c_str());
    
    Variant strCopy = strVal;
    assert( strCopy.isString());
    assert( strcmp(str, strCopy.getString().c_str()) == 0);
    
    Variant intVal(14);
    assert( intVal.isInt() );
    printf("intVal value '%i'\n" , intVal.getInt());
    
    Variant floatVal(14.123f);
    assert( floatVal.isFloat() );
    printf("floatVal value '%f'\n" , intVal.getFloat() );
    
    
    const char* testStr = "My remanent value...";
    
    Variant testAssignment;
    
    {
        Variant value(testStr);
        testAssignment = value;
        
        assert(strcmp(testStr, value.getString().c_str()) == 0);
    }
    assert(strcmp(testStr, testAssignment.getString().c_str())== 0);
    
    
    return true;
}
