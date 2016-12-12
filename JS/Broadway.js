/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <manuel.deneu@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */
import("Element.js");

// Directly use var Application  created bellow


function _Application()
{
    this.prototype = new Element();
    this.id = 0;
    
    
    this.isRunning = function()
    {
        return perform("isRunning" , this.id , 0);
    };
    
    this.getName = function()
    {
        return perform("getApplicationName" , this.id , 0);
    };
    
    this.getTempDirectory = function()
    {
        return perform("getTempDirectory" , this.id , 0);
    };
    
    this.getApplicationFolder = function()
    {
        return perform("getApplicationFolder" , this.id , 0);
    };
    
    /* **** **** **** **** **** **** **** */
    
    this.getDataValue = function( name  )
    {
        return perform("getDataValue" , this.id  ,  name );
    };
    
    this.setDataValue = function( name , value )
    {
        return perform("setDataValue" , this.id  , [ name , value ]);
    };
    
    this.saveConfig = function()
    {
        return perform("saveConfig" , this.id , 0 );
    };
    
    /**/
    
    this.reboot = function()
    {
        return perform("reboot" , this.id , 0 );
    };
    
    this.shutdown = function()
    {
        return perform("shutdown" , this.id , 0 );
    };
    
    /**/
    
    this.getTimeSinceStart = function()
    {
        return perform("getTimeSinceStart" , this.id , 0 );
    };
    
    /* System related calls */
    
    this.getTemperature = function()
    {
        return perform("getTemperature" , this.id , 0 );
    };
    
    this.getGpuMem = function()
    {
        return perform("getGpuMem" , this.id , 0 );
    };
    
    this.getCpu = function()
    {
        return perform("getCpu" ,this.id , 0 );
    };
    
    this.getMem = function()
    {
        return perform("getMem" ,this.id , 0 );
    };
    
    this.getPlateformName = function()
    {
        return perform("getPlateformName" , this.id , 0 );
    };
    
}


var Application = new _Application();
