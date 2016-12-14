//
//  UnitTestBase.hpp
//  Celesta
//
//  Created by Manuel Deneu on 13/12/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//

#ifndef UnitTestBase_hpp
#define UnitTestBase_hpp

#include "../Core/Core.h"

class UnitTestBase
{
public:
    bool run();
    virtual ~UnitTestBase();
protected:
    UnitTestBase(const std::string &name );
    
    virtual bool test() = 0;
private:
    std::string _name;
};

#endif /* UnitTestBase_hpp */
