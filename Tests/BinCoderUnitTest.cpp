//
//  BinCoderUnitTest.cpp
//  Celesta
//
//  Created by Manuel Deneu on 14/12/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//

#include "BinCoderUnitTest.hpp"


BinCoderUnitTest::BinCoderUnitTest():
UnitTestBase("BinCoder")
{
    
}

bool BinCoderUnitTest::test()
{
    BinCoder outCoder;
    
    assert( outCoder.addValueForKey(Variant(134), "key1") );
    assert( outCoder.addValueForKey(Variant("Hello"), "key2") );
    assert( outCoder.addValueForKey(Variant( -6789.23f), "key3") );
    assert( outCoder.addValueForKey(Variant( -789.7890), "key4") );
    
    const GBSize outCoderSize = outCoder.getBufferSize();
    printf("outCoder buffer Size %zi \n" , outCoderSize);
    
    char outBuf[outCoderSize];
    memset(outBuf, 0, outCoderSize);
    
    assert( outCoder.writeBuffer(outBuf));
    
    {
        BinCoder inCoder(outBuf , outCoderSize);
        
        const GBSize inCoderSize = inCoder.getBufferSize();
        
        printf("inCoder buffer Size %zi \n" , inCoderSize);
        
        const Variant v2 = inCoder.getValueForKey("key2");
        
        assert(v2.isNull() == false);
        assert(v2.isString());
        
        printf("v2 : '%s' \n" , v2.getString().c_str());
    }
    
    return false;
}
