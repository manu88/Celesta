//
//  Controller.h
//  JSCenter
//
//  Created by Manuel Deneu on 08/07/2015.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#ifndef __JSCenter__Controller__
#define __JSCenter__Controller__
#include <map>
#include <stdio.h>
#include "../Core/Core.h"


namespace ClassNames
{
    static const std::string JSController  = "JSController";
}


class Controller : public ApplicationBase,
                   public JSMachineDelegate,
                   public TimerDelegate
{
public:
    
    Controller( const std::string &configFile);
    ~Controller();
    
    
    

protected:
    bool applicationWillStart();
    void applicationDidStart();
    void applicationWillStop();
    void applicationDidStop();

    bool reload();
    

    
    void onTime( const Timer &timer);
    
    void functionCalled( const Selector *selector );
    
    
    
    



    void onTime( const std::string &callback);
    
    
    
    
private:

    JSMachine _js;
    
    std::string         _scriptFile;
    
    
};

#endif /* defined(__JSCenter__Controller__) */
