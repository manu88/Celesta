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


/*
 
 loadFontDir( getDataValue("FONTS_Dir") );
 var fonts = listLoadedFonts();
 for (var i = 0;i<fonts.length; i++)
 {
 print("Font '" + fonts[i] + "' \n");
 }
 */


import("GXElement.js");


function GXText()
{
    this.prototype = new GXElement();
    
    this.id = createElement("GXTextRenderer");

    /**/
    
    this.setText = function( text )
    {
        perform("setDisplayedText" , this.id , text );
    };
    
    this.getText = function()
    {
        perform("getDisplayedText" , this.id , 0 );
    };

    /**/
    
    this.setFont = function( fontName )
    {
        return perform("setFont" , this.id , fontName );
    };
    
    this.getFont = function()
    {
        return perform("getFont" , this.id , 0 );
    };
    
    this.setTextColor = function( r , g , b ,a)
    {
        perform("setTextColor" , this.id , [ r , g ,b,a]);
    };
    
    this.getTextColor = function()
    {
        perform("getTextColor" , this.id , 0);
    };
    
    /**/
    
    this.setSize = function( size ) // default is 10.0
    {
        return perform("setSize" , this.id , size);
    };
    
    this.getSize = function()
    {
        return perform("getSize" , this.id , 0);
    };


}
