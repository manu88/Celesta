/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <manuel.deneu@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */

function castAsUIVideo( obj )
{
    obj.prototype = new UIVideo();
    
    return a;
}

function getUIVideoById( id )
{
    var obj.id = id;
    
    castAsUIVideo( obj );
    
    return obj;
}

function UIVideo()
{
    this.prototype = new UIView();
    this.id = createElement("UIVideo");
    
    
    this.setVideoSource = function( file )
    {
        return perform("setVideoSource" , this.id , file);
    };
    this.setAlpha = function(alpha)
    {
        perform("setAlpha" , this.id , alpha );
    };
    this.setPercent = function( percent )
    {
        perform("setPercent" , this.id , percent );
    };
    
    this.start = function()
    {
        return perform("start" , this.id , 0);
    };
    
    this.pause = function()
    {
        return perform("pause" , this.id , 0);
    };
    
    this.resume = function()
    {
        return perform("resume" , this.id , 0);
    };
    
    this.prepare = function()
    {
        return perform("prepare" , this.id , 0);
    };
    
    this.getVideoPlayer = function()
    {
        return perform("getVideoPlayer" , this.id, 0);
    };
    
    this.getVideoLength = function()
    {
        return perform("getVideoLength" , this.getVideoPlayer() , 0 );
    };
    
    this.getCurrentTC = function()
    {
        return perform("getCurrentTC" , this.getVideoPlayer() , 0 );
    };
}
