//
//  GXTouch.cpp
//  JSCenter
//
//  Created by Manuel Deneu on 23/11/2015.
//  Copyright © 2015 Manuel Deneu. All rights reserved.
//

#include "GXTouch.h"

#include "../Env_Variables.h"


// empy impls.
bool GXTouchDelegate::toucheBegan( const std::list<GXTouch> & )
{return true;}
bool GXTouchDelegate::toucheMoved( const std::list<GXTouch> & )
{return true;}
bool GXTouchDelegate::toucheEnded( const std::list<GXTouch> & )
{return true;}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void TouchThread::run()
{
    while (!threadShouldStop())
    {
        const int ret =  _touchEvent.readTouches();
        
        if( ret > 0)
        {
            for (const GXTouch &t : _touchEvent.getTouchesList() )
                _touchesList.push_back( t );
        }
    
    }
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

GXTouchController* GXTouchController::_ins = nullptr;

int GXTouchController::_refCount = 0;

GXTouchController::GXTouchController():
Dispatch::Task( Default_Time_Task , ClassNames::GXTouchController ),
_delegate(nullptr)
{
    setElementName(ClassNames::GXTouchController);
    className = ClassNames::GXTouchController;
    
    Dispatch::getInstance()->add( this );
    
    _touchThread.startThread();
    
}

GXTouchController::~GXTouchController()
{
    _touchThread.stopThread();
}


void GXTouchController::doTask()
{
    /*
     -1 error
     0 no read
     1 began
     2 moved
     3 ended
     */
    
    std::list<GXTouch> _began;
    std::list<GXTouch> _moved;
    std::list<GXTouch> _ended;
    
    const auto list = _touchThread.getTouchesList();
    
    if( !list.empty())
    {
        for (const GXTouch &t : list )
        {
            
            const auto it =std::find_if(_lastTouches.begin(), _lastTouches.end(), [&t](const GXTouch &arg)
                                        {
                                            return arg.id == t.id;
                                        });
            
            if( it == _lastTouches.end() )
            {
                _lastTouches.push_back( t);
            }
            else
            {
                GXTouch &tt = (*it);
                
                // update
                if( t.center.x != -1)
                    tt.center.x = t.center.x;
                if( t.center.y != -1)
                    tt.center.y = t.center.y;
                
                if( t.touchState != tt.touchState )
                {
                    (*it).touchState = t.touchState;
                }
                
                if( (tt.center.x != -1) && (tt.center.y != -1 ))
                {
                    if( tt.touchState == 1)
                    {
                        _began.push_back(tt);
                    }
                    else if( tt.touchState == 2)
                    {
                        const auto itt =std::find_if(_moved.begin(), _moved.end(), [&tt](const GXTouch &arg)
                                                    {
                                                        return arg.id == tt.id;
                                                    });
                        if( itt == _moved.end() )
                            _moved.push_back(tt);
                    }
                }

            }
        }
    }
    else if( !_lastTouches.empty())
    {
        for (const GXTouch &t : _lastTouches )
        {
            if( t.touchState == 3)
            {
                _ended.push_back(t);
            }


        }

        
        _lastTouches.erase( std::remove_if( _lastTouches.begin(),
                                            _lastTouches.end(),
                                            [](const GXTouch &t){return t.touchState== 3;}),
                            _lastTouches.end()
                           );
        
    }
    
    if( !_began.empty() && _delegate)
        _delegate->toucheBegan( _began );

    if( !_moved.empty() && _delegate)
    {
        _delegate->toucheMoved( _moved );
        /*
        printf("##########\n");
        for (const GXTouch &t : _moved )
            printf("Touch %i moved to %i %i \n" , t.id , t.center.x , t.center.y);
         */
    }
    
    if( !_ended.empty() && _delegate)
        _delegate->toucheEnded(_ended);


}
