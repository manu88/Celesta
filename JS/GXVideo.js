/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <manuel.deneu@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */
/*
 GXVideo.js
 
 */

import("GXElement.js");


GXVideoNotification =
{
    VideoLoaded     : 0, // Sent after the player has been loaded and ready to roll.
    VideoWillStart  : 1, // Sent when player enters playing loop.
    VideoDidStart   : 2, // Sent when demuxer sent first frames.
    VideoPaused     : 3, // Sent when pause command performed.
    VideoDidResume  : 4, // Sent when resume command performed.
    VideoWillEnd    : 5, // Sent when video demuxer is end-of-stream.
    VideoDidEnd     : 6, // Sent when player did stop, i.e after flushing last frames.
    VideoDidReachTC : 7, // Notif fired when a registered TC is reached.
} ;

function GXVideo()
{
    this.prototype = new GXElement();
    
    this.id = createElement("GXVideo");
    
    this.setNotificationCallback = function ( func ,target)
    {
        return perform("setNotificationCallback" , this.id , [func , target.id] );
    };

    this.setFileSource = function ( file )
    {
        return perform("setFileSource" , this.id , file);
    };
    
    this.getFileSource = function ()
    {
        return perform("getFileSource" , this.id , 0 );
    };
    
    this.changeFileTo = function( file )
    {
        return perform("changeFileTo" , this.id , file );
    };
    
    this.setLooped = function( looped)
    {
        return perform("setLooped" , this.id , looped );
    };
    this.isLooped = function()
    {
        return perform("isLooped" , this.id , 0 );
    };
    this.prepare = function()
    {
        return perform("prepare" , this.id , looped );
    };
    
    this.release = function()
    {
        perform("release" , this.id , 0 );
    };

    /* **** **** **** **** **** **** */
    
    this.getVideoLength = function()
    {
        return perform("getVideoLength" , this.id , 0 );
    };
    
    this.getCurrentTC = function()
    {
        return perform("getCurrentTC" , this.id , 0 );
    };
    
    this.getRemainingTC = function()
    {
        return perform("getRemainingTC" , this.id , 0 );
    };
    
    /* **** **** **** **** **** **** */
    
    this.seekToTC = function( hh , mm , ss ,ms)
    {
        return perform("seekToTC" , this.id , [ hh , mm , ss , ms] );
    };
    
    this.isStarted = function()
    {
        return perform("isStarted" , this.id , 0 );
    };
    
    this.isPaused = function()
    {
        return perform("isPaused" , this.id , 0 );
    };
    
    this.start = function()
    {
        return perform("start" , this.id , 0 );
    };
    
    this.stop = function()
    {
        perform("stop" , this.id , 0 );
    };
    
    this.pause = function()
    {
        return perform("pause" , this.id , 0 );
    };
    
    this.resume = function()
    {
        return perform("resume" , this.id , 0 );
    };
    
    /* **** **** **** **** **** **** */
    
    this.setVolume = function( vol )
    {
        perform("setVolume" , this.id , vol );
    };
    
    this.getVolume = function()
    {
        return perform("getVolume" , this.id , 0);
    };
}
