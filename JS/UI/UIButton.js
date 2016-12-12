/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <manuel.deneu@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */



UIButtonType =
{
    Momentary : 0, //default
    Toggle    : 1
};

function UIButton()
{
    this.prototype = new UIView();
    this.id = createElement("UIButton");
    
    this.setText = function( text )
    {
        perform("setText" , this.id , text);
    };
    
    
    /*
     See UIButtonType below
     */
    this.setType = function( type)
    {
        perform("setType" , this.id , type);
    };
    
    this.getType = function()
    {
        return perform("getType" , this.id , 0);
    };
}


function castAsUIButton( obj )
{
    obj.prototype = new UIButton();
    
    return a;
}

function getUIButtonById( id )
{
    var obj.id = id;
    
    castAsUIButton( obj );
    
    return obj;
}

