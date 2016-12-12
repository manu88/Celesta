/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <manuel.deneu@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */

function UISlider()
{
    this.prototype = new UIView();
    this.id = createElement("UISlider");
    
    this.setValue = function( val)
    {
        perform("setValue" , this.id , val);
    };
    
    this.getValue = function( )
    {
        return perform("getValue" , this.id , 0);
    };

}

function castAsUISlider( obj )
{
    obj.prototype = new UISlider();
    
    return a;
}

function getUISliderById( id )
{
    var obj.id = id;
    
    castAsUISlider( obj );
    
    return obj;
}

