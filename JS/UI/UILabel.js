/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <manuel.deneu@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */


function UILabel()
{
    this.prototype = new UIView();
    this.id = createElement("UILabel");
    
    this.setText = function( text )
    {
        perform("setText" , this.id , text);
    };

}


