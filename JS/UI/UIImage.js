/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <manuel.deneu@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */


function UIImage()
{
    this.prototype = new UIView();
    this.id = createElement("UIImage");
    

    
    this.setImageSource = function( imgFile )
    {
        return perform ("setImageSource" , this.id , imgFile );
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


function castAsUIImage( obj )
{
    obj.prototype = new UIImage();
    
    return a;
}

function getUIImageById( id )
{
    var obj.id = id;
    
    castAsUIImage( obj );
    
    return obj;
}


