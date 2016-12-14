//
//  TimerUnitTest.cpp
//  Celesta
//
//  Created by Manuel Deneu on 13/12/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//

#include "TimerUnitTest.hpp"


static  void testAsync(GBRunLoop* runLoop , void* data)
{
    printf("Test Async \n");
}

class MyTimer : public TimerDelegate
{
    void onTime()
    {
        static int accum = 0;
        
        printf("ontime1\n");
        accum++;
        
        if( accum == 10 )
        {
            printf("Send stop\n");
            getTimer()->getRunLoop()->stop();
        }
        
        getTimer()->getRunLoop()->dispatchAsync(testAsync, nullptr);
    }
    
    
};

TimerUnitTest::TimerUnitTest():
UnitTestBase("Timer")
{
    
}

bool TimerUnitTest::test()
{
    
    RunLoop runLoop;
    Timer timer1;
    timer1.setInterval(1000);
    
    runLoop.addTimer( timer1);
    
    MyTimer delegate;
    timer1.setDelegate(&delegate);
    
    runLoop.run();
    
    return true;
}
