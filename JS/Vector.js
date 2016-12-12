/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <manuel.deneu@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */

import("Element.js");

// std::vector like class
function Vector()
{
    
    this.prototype = new Element();
    
    this.id = createElement("Vector");
    
    this.size = function()
    {
        return perform("size" , this.id , 0);
    };
    
    this.clear = function()
    {
        perform("clear" , this.id , 0);
    };
    
    this.empty = function()
    {
        return perform("empty" , this.id , 0);
    };

    this.at = function( pos)
    {
        return perform("at" , this.id , pos );
    };
    this.find = function( value)
    {
        return perform("find" , this.id , value );
    };
    this.push_back = function( value)
    {
        return perform("push_back" , this.id , value );
    };
    
    this.remove = function( value)
    {
        return perform("remove" , this.id , value );
    };


}

