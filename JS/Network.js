/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <manuel.deneu@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */
/*
 Network.js
 
 */

import("Element.js");

function _NetworkController()
{
    this.prototype = new Element();
    this.id = createElement("NetworkController");
    
    /* form function onOsc(ip , port , address , args)*/
    this.setOscCallback = function( func , target )
    {
        return perform("setOscCallback" , this.id , [ func , target.id ] );
    };

    this.send = function( addr , port , header , vars)
    {
        return perform("sendOSC" , this.id , [ addr , port , header , vars ] );
    };
    
    this.sendBroadcast = function( addr , port , header , vars)
    {
        return perform("sendOSCBroadcast" , this.id , [ addr , port , header , vars ] );
    };


    this.addPort = function ( port)
    {
        return perform("addPort" , this.id , port  );
    };

    this.removePort = function ( port )
    {
        return perform("removePort" , this.id , port  );
    };
    
    this.stop = function ()
    {
        return perform("stop" , this.id , port  );
    };

    
    /* **** **** **** **** **** **** **** **** **** **** **** **** **** */

    this.getIp = function ()
    {
        var ip = this.getEthernetIp();
        
        if (ip == undefined )
            ip = this.getWlanIp();
        
        return ip;
    };

    this.getHostName = function()
    {
        return perform("getHostName" , this.id , 0  );
    };

    this.getWlanIp = function()
    {
        var ip;
        
        if( Application.getPlateformName() == "RaspberryPi")
            ip = perform("getIp" , this.id , "wlan0");
        
        else if( Application.getPlateformName() == "MacOs")
            ip = perform("getIp" , this.id , "en1");

        else
            print("Error : wlan name undefined for " +  Application.getPlateformName() );
        
        if ( 0 !== ip.length)
            return ip;
    };

    this.getEthernetIp = function()
    {
        var ip;
        
        if( Application.getPlateformName() == "RaspberryPi")
            ip = perform("getIp" , this.id , "eth0");
        
        else if( Application.getPlateformName() == "MacOs")
            ip = perform("getIp" , this.id , "en0");

        else
            print("Error : ethernet name undefined for " +  Application.getPlateformName() );
        if ( 0 !== ip.length)
            return ip;

    };
}

var NetworkController = new _NetworkController();