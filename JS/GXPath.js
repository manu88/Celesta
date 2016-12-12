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

function GXPath()
{
    this.prototype = new GXElement();
    
    this.id = createElement("GXAsyncPainter");
    
    /* **** */
    
    this.clear = function()
    {
        perform("clear" , this.id , 0);
    };

    this.stroke = function()
    {
        perform("stroke" , this.id , 0);
    };
    
    this.fill = function()
    {
        perform("fill" , this.id , 0);
    };
    
    /* **** */
    
    this.setFillColor = function( r , g , b , a)
    {
        perform("setFillColor" , this.id , [r , g , b , a]);
    };
    
    this.setStrokeColor = function( r , g , b , a)
    {
        perform("setStrokeColor" , this.id , [r , g , b , a]);
    };
    
    this.setStrokeWidth = function(  width )
    {
        perform("setStrokeWidth" , this.id , width);
    };
    
    this.addRect = function( x , y , w , h )
    {
        perform("addRect" , this.id , [x , y , w , h] );
    };
    
    this.addRoundedRect = function( x , y , w , h , arcW , arcH )
    {
        perform("addRoundedRect" , this.id , [x , y , w , h , arcW , arcH] );
    };
    
    this.addEllipse = function( x , y , w , h )
    {
        perform("addEllipse" , this.id , [x , y , w , h] );
    };
    
    this.addLine = function( x0 , y0 , x1 , y1 )
    {
        perform("addLine" , this.id , [x0 , y0 , x1 , y1] );
    };
    
    /* **** */
    
    

}
