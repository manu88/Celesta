/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <manuel.deneu@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */

InitClass("UIModalAlert");

function UINotification( title)
{
    return performStatic("showModalAlert" , "UIModalAlert" ,  title);
}
/*
function UINotification()
{
    this.prototype = new UIBase();
    this.id = createElement("UINotification");
    

}
*/

