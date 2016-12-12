/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <manuel.deneu@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */

import("Element.js");

InitClass("Url");


function getPendingCount()
{
    return performStatic("getPendingCount" , "Url" , 0);
}

function Url( url , name )
{
    this.prototype = new Element();
    this.id = createElement("Url");
    
    
    this.setName = function ( name )
    {
        perform ("setName" , this.id , name);
    };
    this.getName = function ()
    {
        perform ("getName" , this.id , 0);
    };
    
    this.setName(name); // call at init.
    
    this.getUrl = function()
    {
        return perform("getUrl" , this.id , 0 );
    };
    this.setUrl = function( url )
    {
        return perform("setUrl" , this.id , url );
    };
    this.setUrl( url ); // call at init.
    
    
    this.getState = function()
    {
        return perform("getState" , this.id , 0);
    };
    
    /* **** **** **** */
    
    this.setFolder = function( folder)
    {
        perform("setFolder" , this.id , folder );
    };
    
    this.getFolder = function()
    {
        return perform("getFolder" , this.id , 0 );
    };
    
    this.getRessourcePath = function()
    {
        return perform("getRessourcePath" , this.id , 0 );
    };
    

    this.setThreaded = function( useThread /*bool*/)
    {
        perform("setThreadedFetch" , this.id , useThread );
    };
    
    this.isThreaded = function()
    {
        return perform("isThreadedFetch" , this.id , 0 );
    };
    
    
    this.startFetchContent = function(  )
    {
        return perform("startFetchContent" , this.id , 0 );
    };


    
    this.setPercentCallback = function( func , target )
    {
        return perform("setPercentCallback" , this.id , [ func , target.id ] );
    };
    
    this.setEndCallback = function( func , target )
    {
        return perform("setEndCallback" , this.id , [ func , target.id ] );
    };
    




}

