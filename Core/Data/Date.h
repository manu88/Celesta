//
//  Date.hpp
//  JSCenter
//
//  Created by Manuel Deneu on 31/10/2015.
//  Copyright © 2015 Manuel Deneu. All rights reserved.
//

#ifndef Date_hpp
#define Date_hpp

#include <stdio.h>
#include <time.h>
#include <string>

namespace Date
{
    inline const std::string currentDateTime()
    {
        time_t     now = time(0);
        struct tm  tstruct;
        char       buf[80];
        tstruct = *localtime(&now);

        strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
        
        return buf;
    }
}

#endif /* Date_hpp */
