/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <manuel.deneu@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */
/*
*/

import("Element.js");


SerialSpeed =
{
    Serial_0      : 0, // unspecified
    Serial_1200   : 1,
    Serial_2400   : 2,
    Serial_4800   : 3,
    Serial_9600   : 4,
    Serial_19200  : 5,
    Serial_38400  : 6,
    Serial_57600  : 7,
    Serial_115200 : 8
};



function Serial()
{
    this.prototype = new Element();
    this.id = createElement("SerialEvent");
    
    this.getSerialDevicesList = function()
    {
        return perform("getSerialDevicesList" , this.id , 0);
    };
    
    this.setPort = function( port )
    {
        perform("setPort" , this.id , port);
    };
    
    this.getPort = function()
    {
        perform("getPort" , this.id , 0);
    };
    
    this.setSpeed = function( bauds /* see SerialSpeed above */)
    {
        return perform("setSpeed" , this.id , bauds );
    };
    
    this.getSpeed = function()
    {
        return perform("getSpeed" , this.id , O );
    };
    
    this.isOpen = function()
    {
        return perform("isOpen" , this.id , O );        
    };
    
    /* *** */
    
    this.send = function( data )
    {
        return perform("send" , this.id , data);
    };
    
    /* *** */
    
    this.setBytesToRead = function( numBytes)
    {
        perform("setBytesToRead" , this.id , numBytes );
    };
    
    this.getBytesToRead = function( )
    {
        perform("getBytesToRead" , this.id , 0 );
    };
    
    this.read = function()
    {
        return perform("read" , this.id , 0);
    };
    
    

}





