/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <manuel.deneu@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */

import("Broadway.js");

function _AudioMixer()
{
    this.test = function()
    {
        var c = Application.getChildren();
        
        for (var i = 0 ; i< c.length ; i++)
        {
            var child = getElementById( c[i]);
            
            
            
            if( child.conformsTo( 1 )  )
                print(" \nChild : " + child.getElementName() + "\n");
            
            
        }
    };
}

var AudioMixer = new _AudioMixer();
