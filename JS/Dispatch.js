/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <manuel.deneu@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */
import("Timer.js");
import("Element.js");

function dispatchAfter( timeInMs , selector) // -> Bool success
{
    var t = new Timer( timeInMs , 0 , selector );
    
    return t.start();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

function IPCListener()
{
    this.prototype = new Element();
    this.id = createElement("IPCListener");
    
    /* *** *** *** *** *** *** */
    
    this.connectWithFile = function( file )
    {
        return perform("connectWithFile" , this.id , file );
    };
    
    this.connectWithKey = function( key )
    {
        return perform("connectWithKey" , this.id , key );
    };
    
    this.isConnected = function()
    {
        return perform("isConnected" , this.id , 0);
    };

    /* *** *** *** *** *** *** */
    
    this.setTypeToRead = function( type )
    {
        return perform("setTypeToRead" , this.id , type );
    };
    
    this.getTypeToRead = function()
    {
        return perform("getTypeToRead" , this.id , 0 );
    };
    
    // function form : function onIpc( senderID ,type, msg )
    this.setNotificationSelector = function( func , target /* Element class*/ )
    {
        return perform("setNotificationSelector" , this.id , [ func , target.id ] );
    };
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

function IPCSender()
{
    this.prototype = new Element();
    this.id = createElement("IPCSender");
    
    this.send = function( type, msg )
    {
        return perform("send" , this.id , [  type , msg ] );
    };
    /* *** *** *** *** *** *** */
    
    this.connectWithFile = function( file )
    {
        return perform("connectWithFile" , this.id , file );
    };
    
    this.connectWithKey = function( key )
    {
        return perform("connectWithKey" , this.id , key );
    };
    
    this.isConnected = function()
    {
        return perform("isConnected" , this.id , 0);
    };
    
    /* *** *** *** *** *** *** */
}



