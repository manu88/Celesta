/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <manuel.deneu@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */

import("GXScene.js");
import("GXImage.js");
import("GXPath.js");
import("GXText.js");


import("UI/UIButton.js");
import("UI/UISlider.js");
import("UI/UIImage.js");
import("UI/UILabel.js");
import("UI/UIVideo.js");
import("UI/UINotification.js");
import("UI/UIStoryBoard.js");

function UIView()
{
    this.prototype = new GXElement();
    
    this.setTarget = function(selector , target)
    {
        return perform("setTarget" , this.id , [selector , target.id]);
    };
    
    this.setPosition = function( x , y )
    {
        perform("moveTo" , this.id , [ x , y]);
    };
    /*
     function viewWillAppear( viewID );
     function viewDidAppear( viewID );
     function viewWillDisappear( viewID );
     function viewDidDisappear( viewID );
     */

}

function castAsUIWindow( obj )
{
    obj.prototype = new UIWindow();
    
    return a;
}

function getUIWindowById( id )
{
    var obj.id = id;
    
    castAsUIWindow( obj );
    
    return obj;
}

function UIWindow()
{
    this.prototype = new UIView();
    
    this.id = createElement("UIWindow");
    

    this.addElement = function( element)
    {
        perform("addElement" , this.id , element.id );
    };
    
    this.getChildNamed = function (name)
    {
        return getUIWindowById( perform("getChildNamed" , this.id , name ) );
    };
    
    this.setViewController = function (element )
    {
        perform("setViewController" , this.id , element.id);
    };
}



