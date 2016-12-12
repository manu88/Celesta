//
//  main.cpp
//  JSCenter
//
//  Created by Manuel Deneu on 08/07/2015.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#include <iostream>
/*
 ENABLE_ELEMENT_SELECTOR
 USE_WEB_SERVER
 USE_JAVA_INTERPRETER
 USE_NETWORK
 USE_GPIO
 USE_GRAPHICS
 USE_GRAPHICS_HELPERS
 USE_GRAPHICS_EXTRA
 HAVE_JPEG_LIB
*/
#include "../Core/Core.h"

#include "Controller.h"


int main(int argc, const char * argv[])
{

    ScopedPlateformConfig p;
    Log::addLocalLogger();
    
    std::string conf = std::string( "config.txt" );
    
    if (argc >1)
        conf = argv[1];
    
    /**/
    {
        Controller controller( conf );
        
        if ( !controller.start() )
        {
            Log::log("ERROR : Return to OS ");
        }
    }
    
    
    Log::cleanupLogger();
    
    printf("\nReturned to OS\n");
    
    return 0;
}
