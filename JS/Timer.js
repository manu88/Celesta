/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <manuel.deneu@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */
import("Element.js");
/*
 Timer.js
 
 Chrono Class
 Timer  Class
 
 */

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

function Chrono()
{
    
    this.prototype = new Element();
    
    this.id = createElement("Chrono");

    this.reset = function()
    {
        perform("reset" , this.id , 0);
    };
    
    this.elapsedTimeInMs = function()
    {
        return perform("elapsedTimeInMs" , this.id , 0);
    };
    
    // Use this function to measure deltas
    this.timeSinceLastMeasure = function()
    {
        return perform("timeSinceLastMeasure" , this.id , 0);
    };
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
    Timers
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
function getActiveTimersCount()
{
    return perform("getActiveTimersCount" , "Dispatch" , 0 );
}

function Timer( delay , period , callback )
{

    this.timerId = -1;

    this.delay    = delay;
    this.period   = period;

    this.callback = callback;
    
    this.chrono = new Chrono();
    
    this.isRunning = function()
    {
        return timerExists( this.timerId );
    };
    
    this.getIdentifier = function()
    {
        return timerGetIdentifier( this.timerId );
    };
    
    /* Time measure */
    
    /* Elapsed time since last update occured */
    
    this.timeSinceLastUpdate = function()
    {
        return this.chrono.timeSinceLastMeasure();
    };
    
    this.timeSinceStart = function()
    {
        return this.chrono.elapsedTimeInMs();
    };
    
    this.resetTimeMeasure = function()
    {
        this.chrono.reset();
    };
    
    this.start = function()
    {

        if( ! this.isRunning() )
        {
            this.timerId = addTimer( this.delay ,this.period , this.callback );

            this.chrono.reset();
            
            return this.timerId != -1;
            

        }
        
        return false;
    };

    this.stop = function()
    {

        if( this.isRunning() )
        {
            return removeTimer( this.timerId );
            this.timerId = -1;
        }
        
        return false;
    };
    
    /* In case of a task taking more time than its periodicity, 
       this option adjusts the timer's period to the time taken */
    this.setFlexible = function()
    {
        timerSetFlexible( this.id);
    };
}
