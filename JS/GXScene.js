/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <manuel.deneu@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */
/*
 GXImage.js
 
 */


import("GXElement.js");

function GXScene()
{
    this.prototype = new GXElement();
    
    this.id = createElement("GXScene");
    
    
    this.addElement = function ( element )
    {
        perform("addElement" , this.id , element.id );
    };

    this.removeElement = function ( element )
    {
        perform("removeElement" , this.id , element.id );
    };
    
    this.removeAllElements = function()
    {
        perform("removeAllElements" , this.id , 0 );
    };
    
    this.getNumChildren = function()
    {
        return perform("getNumChildren" , this.id , 0 );
    };
    
    this.getChildAt = function( index)
    {
        return getGXElementById( perform("getChildAt" , this.id , index ) );
    };
    
    this.getChildNamed = function (name)
    {
        return getGXElementById( perform("getChildNamed" , this.id , name ) );
    };
    
    this.childAtPoint = function( x , y)
    {
        return getGXElementById( perform("childAtPoint" , this.id , [ x , y] ) );
    };


}
