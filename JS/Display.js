/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <manuel.deneu@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */
/*
 Display.js
 */

DisplayNotification =
{
    HDMI_UNPLUGGED           : (1 << 0), // <HDMI cable is detached
    HDMI_ATTACHED            : (1 << 1), // <HDMI cable is attached but not powered on
    HDMI_DVI                 : (1 << 2), // <HDMI is on but in DVI mode (no audio)
    HDMI_HDMI                : (1 << 3), // <HDMI is on and HDMI mode is active
    HDMI_HDCP_UNAUTH         : (1 << 4), // <HDCP authentication is broken (e.g. Ri mismatched) or not active
    HDMI_HDCP_AUTH           : (1 << 5), // <HDCP is active
    HDMI_HDCP_KEY_DOWNLOAD   : (1 << 6), // <HDCP key download successful/fail
    HDMI_HDCP_SRM_DOWNLOAD   : (1 << 7), // <HDCP revocation list download successful/fail
    HDMI_CHANGING_MODE       : (1 << 8), // <HDMI is starting to change mode, clock has not yet been set
    
    SDTV_UNPLUGGED           : 1 << 16, //  <SDTV cable unplugged, subject to platform support
    SDTV_ATTACHED            : 1 << 17, //  <SDTV cable is plugged in
    SDTV_NTSC                : 1 << 18, //  <SDTV is in NTSC mode
    SDTV_PAL                 : 1 << 19, //  <SDTV is in PAL mode
    SDTV_CP_INACTIVE         : 1 << 20, //  <Copy protection disabled
    SDTV_CP_ACTIVE           : 1 << 21, //   <Copy protection enabled
    
    DISPLAY_ACTIVE           : 1 << 30 // display active 1073741824
};

import("Element.js");

function _DisplayController()
{
    this.prototype = new Element();
    this.id = createElement("DisplayController");


    this.clearScreen = function()
    {
        perform("clearScreen" , this.id , 0);
    };

    // ex. function foo( displayNotif ) {}
    this.setCallback = function( func , target )
    {
        return perform("setCallback" , this.id , [ func , target.id ] );
    };

    this.setDisplayedElement = function( element )
    {
        perform("setDisplayedElement" , this.id , element.id );
    };

    this.setFrameRate = function( rate )
    {
        perform("setFrameRate" , this.id , rate);
    };
    
    this.getCurrentFrameRate = function()
    {
        return perform("getCurrentFrameRate" , this.id , 0);
    };
    
    this.getWidth = function()
    {
        return this.getCurrentMode()[2];
    };
    
    this.getHeight = function()
    {
        return this.getCurrentMode()[3];
    };

    /* *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** */

    this.powerOn = function()
    {
        perform("powerOn" , this.id , 0);
    };

    this.powerOff = function()
    {
        perform("powerOff" , this.id , 0);
    };

/* *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** */
/*
 mode : array;
 0 - native : true/false
 1 - type   :     Display_Invalid = -1 ; Display_SD = 0 ; Display_HD = 1
 2 - width
 3 - height
 4 - framerate
 5 - aspectRatio 
 */
    this.getCurrentMode = function ()
    {
        return perform("getCurrentMode" , this.id , 0);
    };

    this.getAvailableVideoMode = function()
    {
        return perform("getAvailableVideoMode" , this.id , 0);
    };

    this.setVideoModeTo = function ()
    {
        return perform("setVideoModeTo" , this.id , 0);
    };
}

var DisplayController = new _DisplayController();
