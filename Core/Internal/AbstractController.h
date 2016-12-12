//
//  AbstractController.h
//  Broadway_test
//
//  Created by Manuel Deneu on 18/09/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_test__AbstractController__
#define __Broadway_test__AbstractController__

#include <iostream>
#include <unordered_set>
#include "Thread.h"

class AbstractDelegate;

class AbstractController 
{
public:
    
    friend class Controllers;

    virtual bool start() = 0;
    virtual bool stop()  = 0;
    
    const std::string getName() const noexcept
    {
        return _controllerName;
    }
    
    bool isReady() const noexcept
    {
        return _isReady;
    }
    
    bool isInactive() const noexcept
    {
        return _isInactive;
    }
    
    bool isActive() const noexcept
    {
        return !_isInactive;
    }
    
    static bool allControllersUnReady() noexcept;
    static bool allControllersReady()   noexcept;
    
    
    
protected:
    AbstractController(const std::string &name);
    virtual ~AbstractController();
    
    void setReady() noexcept
    {
        _isReady = true;
        _isInactive = false;
    }
    
    void setUnReady() noexcept
    {
        _isReady = false;
    }

    void setInactive() noexcept
    {
        _isInactive = true;
    }

private:
    static bool waitForAllControllersToBeReady(int timeOut = 10 /*SECS*/) noexcept;
    static bool waitForControllerToBeReady( AbstractController*controller , int timeOut = 10 /*SECS*/);
    static bool waitForControllerToFinish( AbstractController*controller , int timeOut = 10 /*SECS*/);
    
    static int countReadyControllers() noexcept;
    
    static bool waitForAllControllersToFinish(int timeOut = 10 /*SECS*/);

    
    
    
    bool _isReady;
    bool _isInactive;
    
    std::string _controllerName;

    static std::unordered_set< AbstractController *> s_controllers;
    
    
};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class Controllers
{
public:
    static bool waitForAllControllersToBeReady(int timeOut = 10 /*SECS*/) noexcept
    {
        return AbstractController::waitForAllControllersToBeReady( timeOut );
    }
    
    static bool waitForControllerToBeReady( AbstractController*controller , int timeOut = 10 /*SECS*/)
    {
        return AbstractController::waitForControllerToBeReady( controller , timeOut );
    }
    
    static bool allControllersReady() noexcept
    {
        return AbstractController::allControllersReady();
    }
    
    static int countReadyControllers() noexcept
    {
        return AbstractController::countReadyControllers();
    }
    
    static bool waitForAllControllersToFinish(int timeOut = 10 /*SECS*/)
    {
        return  AbstractController::waitForAllControllersToFinish( timeOut );
    }
    static bool allControllersUnReady() noexcept
    {
        return  AbstractController::allControllersUnReady();
    }
    
    
};

#endif /* defined(__Broadway_test__AbstractController__) */









