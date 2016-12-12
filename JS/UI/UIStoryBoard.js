/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <manuel.deneu@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */

import("Element.js");


function castAsUIView( obj )
{
    obj.prototype = new UIView();
    
    return a;
}

function getUIViewById( id )
{
    var obj.id = id;
    
    castAsUIView( obj );
    
    return obj;
}

function UIStoryBoard()
{
    this.prototype = new Element();
    this.id = createElement("UIStoryBoard");
    
    
    this.parseJSONFile = function( file )
    {
        return perform("parseJSONFile" , this.id , file);
    };
    
    this.getNumWindows = function()
    {
        return perform("getNumWindows" , this.id , 0);
    };
    
    this.getWindowNamed = function( name )
    {
        return getUIWindowById(perform("getWindowNamed" , this.id , name));
    };
    
    this.saveJSON = function( file )
    {
        return perform("saveJSON" , this.id , file);
    };
}


