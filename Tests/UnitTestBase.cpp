//
//  UnitTestBase.cpp
//  Celesta
//
//  Created by Manuel Deneu on 13/12/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//

#include "UnitTestBase.hpp"


UnitTestBase::UnitTestBase(const std::string &name )
{
    _name = name;
}

UnitTestBase::~UnitTestBase()
{
    
}


bool UnitTestBase::run()
{
    printf("## Start test : %s ##\n" , _name.c_str());
    return test();
}
