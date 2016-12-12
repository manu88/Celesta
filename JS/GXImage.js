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

function GXImage()
{
    this.prototype = new GXElement();
    
    this.id = createElement("GXImage");

    this.setFileSource = function ( file )
    {
        return perform("setFileSource" , this.id , file );
    };
    
    this.getFileSource = function ()
    {
        return perform("getFileSource" , this.id , 0 );
    };
    
    
    this.setScale = function (w , h)
    {
        perform("setScale" , this.id , [ w , h]);
    };
    
    this.getScale = function()
    {
        return perform("getScale" , this.id , 0);
    };

}
