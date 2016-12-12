/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <manuel.deneu@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */
/*

    This Class defines basic operations on displayed elements (GXElement class).

    This is a base class. The following objects are constructed from this one:
        - GXScene
        - GXVideo
        - GXText
        - GXPath
        - GXimage
    You don't have to import this header if you're using these Objects, it will be automaticaly added.
*/


import("Element.js");

InitClass("GXElement"); // init static calls for GXElement class


function castAsGXElement( obj )
{
    obj.prototype = new GXElement();
    
    return a;
}

function getGXElementById( id )
{
    var obj.id = id;
    
    castAsGXElement( obj );
    
    return obj;
}

function GXElement() 
{
    this.prototype = new Element();

    this.setNeedsDisplay = function()
    {
        perform("setNeedsDisplay" , this.id , 0 );
    };
    
    this.startContinuousRendering = function()
    {
        perform("startContinuousRendering" , this.id , 0);
    };
    
    this.stopContinuousRendering = function()
    {
        perform("stopContinuousRendering" , this.id , 0);
    };
    
    this.setVisible = function( visible)
    {
        perform("setVisible" , this.id , visible );
    };
    
    this.setLayer = function( num)
    {
        perform("setLayer" , this.id , num );
    };
    
    this.setBounds = function( x , y , w , h)
    {
        perform("setBounds" , this.id , [ x , y , w , h] );
    };
    
    this.getBounds = function()
    {
        return perform("getBounds" , this.id , 0);
    };
    
    
    this.moveTo = function(x , y)
    {
        perform("moveTo" , this.id , [x,y] );
    };
    
    this.moveOf = function(x , y)
    {
        perform("moveOf" , this.id , [x,y] );
    };
    
    this.setAlpha = function( alpha )
    {
        return perform("setAlpha" , this.id , alpha );
    };
    
    this.getAlpha = function()
    {
        return perform("getAlpha" , this.id , 0 );
    };
    
    this.setTransparency = function(transparent)
    {
        perform("setTransparency" , this.id , transparent);
    };
    
    this.setOpaque = function( opaque )
    {
        this.setTransparency( !opaque );
    };
    
    this.isOpaque = function()
    {
        return perform("isOpaque" , this.id , 0);
    };
    
    this.isTransparent = function()
    {
        return !perform("isOpaque" , this.id , 0);
    };
    
    this.setBackgroundColor = function(r , g , b ,a)
    {
        perform("setBackgroundColor" , this.id , [r , g , b ,a ]);
    };
    
    this.getBackgroundColor = function()
    {
        return perform("getBackgroundColor" , this.id , 0);
    };
    
    /**/
    
    this.runAnimation = function( anim )
    {
        return perform("runAnimation" , this.id , anim);
    };
    
    /* Same as above, but will post a message on the main thread at completion */
    this.runAnimationWithCompletion = function( anim , completionCallback )
    {
        return perform("runAnimationWithCompletion" , this.id , [anim , completionCallback ]);
    };
    
    this.stringify = function()
    {
        return perform("stringify" , this.id , 0);
    };
}

function WaitComponent()
{
    this.prototype = new GXElement();
    
    this.id = createElement("WaitComponent");
}



