/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <manuel.deneu@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */


/*
 
 These constants need to be defined in config file:
 
 OUT_MUSEO_PORT : output OSC port
 IN_MUSEO_PORT  : input  OSC port
 OUT_MUSEO_IP   : museo server's IP
 
 MUSEO_NAME     : name of the device
 */

import("Network.js");
import("Timer.js");

var outMuseoPort = getDataValue("OUT_MUSEO_PORT");
var inMuseoPort  = getDataValue("IN_MUSEO_PORT");
var outIp        = getDataValue("OUT_MUSEO_IP");

var museoName    = getDataValue("MUSEO_NAME");

var dtStats      = Integer.parseInt( getDataValue("MUSEO_STATS_LOG_MINS") );

function loadMuseoConfig()
{
    if( outMuseoPort == undefined)
        outMuseoPort = 10000;
    
    if( inMuseoPort == undefined)
        inMuseoPort = 9000;
    
    if( museoName == undefined )
        museoName = "Museo_device";
    
    if (dtStats == undefined )
        dtStats = 60;
    
    if(!addPort( inMuseoPort ))
        log( "Unable to add in port OSC "+inMuseoPort+"\n");
    
    setOscCallback("museo_osc");

    var timer = new Timer( dtStats*60000 , dtStats*60000 , "statsCallback" );
    timer.start();
    
}

function statsCallback()
{
    log("#####################");
    log("STATS : ");
    log("TIME  : "+ getTimeSinceStart() );
    log("CPU   : "+ getCpu() );
    log("RAM   : "+ getMem() );
    log("TEMP  : "+ getTemperature() );
    log("#####################");

}


function museo_osc(ip , port , address , args)
{

    if( address == "/getIp")
    {
        send( outIp , outMuseoPort , "/myIp" , [getIp() , getHostName() , museoName ] );
    }
    else if (address == "/setMuseoName")
    {
        museoName = args[0];
        setDataValue("MUSEO_NAME" , museoName );
    }
    else if (address == "/setServerIp")
    {
        outIp = args[0];
        setDataValue("OUT_MUSEO_IP" , outIp);
    }
    else if (address == "/quit")
    {
        quit();
    }
    else if (address == "/reboot")
    {
        reboot();
    }
    else if (address == "/shutdown")
    {
        shutdown();
    }
    else if( address == "/selector")
    {
   
        var select =args[0];
        var obj = args[1];
        

        args.remove( select );
        args.remove( obj );
        
        var ret = perform( select , obj, args );
        
        if (ret == undefined )
            ret = "undefined";
        
        send( outIp , outMuseoPort , "/"+museoName+"/"+args[0]+"/" , ret );
    }
    
    else if( address == "/logElementsTree")
    {
        LogElementTree();
    }
    /**/
    else
        user_osc( ip , port , address , args);
}
