/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <manuel.deneu@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */
/*
*/

import("Element.js");

function addLocalLogger()
{
    perform("addLocalLogger" , 0 , 0);
}

function addFileLogger()
{
    perform("addFileLogger" , 0 ,0);
}

function log(txt)
{
    perform("log" , 0 , txt);
}






