/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <manuel.deneu@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */


var InputDirect     = 0;
var InputPullDown   = 1;
var InputPullUp     = 2;

import("Element.js");

/*
 
 var gpio = new InterfaceController();
 gpio.setCallback("gpioIn"); // function gpioIn( pin , value)
 gpio.addGPIO( 12 , InputPullDown );
 gpio.writeGPIO( 11 , 1);
 */

function InterfaceController()
{
    this.prototype = new Element();
    this.id = createElement("InterfaceController");
    
    this.setCallback = function( callback /* str */)
    {
        return setInterfaceCallback( callback );
    };
    
    this.addGPIO = function( pin , mode)
    {
        return perform("addGPIO" , this.id , [ pin , mode ]);
    };
    
    this.writeGPIO = function( pin , value )
    {
        return perform("writeGPIO" , this.id , [ pin , value ]);
    };
    this.listenKeyboard = function()
    {
        return perform("listenKeyboard" , this.id , 0);
    };

    
    
}