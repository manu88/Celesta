/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <manuel.deneu@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */

import("Element.js");

InitClass("WebServer");

/*
 import("Web.js");
 
 
 var server = new WebServer( 8080);
 
 function getRequest( ip, port , address, arguments )
 {
    print("Get from " + ip + " on port " + port + " address :"+address +"\n");
 
    for (var i = 0 ; i < arguments.length ; i++)
    {
        print("DataName = " + arguments[i][0] );
        print("\nValue    = '" + decodeUrl( arguments[i][1] )  +"'");
        print("\n");
    }
 
 
    return "salut";
 }
 
 function applicationDidStart()
 {
    addLocalLogger();
 
    setWebCallback( "getRequest");
    server.setRootDirectory( getDataValue("WebDir"));
    server.start();
 
 }
 
 
 function applicationWillStop()
 { }
 
 */

function decodeUrl( text )
{
    return performStatic("getDecodedUrl" , "WebServer" , text );
}

function WebServer( port )
{
    this.prototype = new Element();
    this.id = createElement("WebServer");
    
    this.port = port;
    
    this.start = function()
    {
        perform("setPort" , this.id , this.port );
        return perform("start" , this.id , 0);
    };
    
    this.setRootDirectory = function ( folder )
    {
        perform("setWorkingDirectory" , this.id , folder );
    };
}

