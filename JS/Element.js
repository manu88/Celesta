/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <manuel.deneu@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */
/*

    This Class defines basic operations on elements (Element class).

    This is a base class. The following objects are constructed from this one:
 
    You don't have to import this header if you're using these Objects, it will be automaticaly added.
*/

InitClass("Element"); // init static calls for GXElement class


function LogElementTree( /*bool*/ additionalInfos )
{
    return performStatic("LogElementTree" , "Element" , additionalInfos );
}

function getElementById( id )
{
    var obj.id = id;
 
    castAsElement( obj );
    
    return obj;
}

function getElementByName( name )
{
    return performStatic("getElementByName" , "Element" , Name );
}

/* **** **** **** **** **** **** **** **** */

function castAsElement( obj )
{
    obj.prototype = new Element();

    return a;
}

/* **** **** **** **** **** **** **** **** */

function Element()
{

    this.id = -1; // set by sublasses
    
    /*
     if(!a.setElementName("Dispatch"))
     {
     print("\n name already exists! Change to : "+a.getElementName() +"\n");
     
     }
     */
    this.setElementName = function(name)
    {
        return perform("setElementName" , this.id , name);
    };
    
    this.getElementName = function()
    {
        return perform("getElementName"  , this.id , 0);
    };
    
    this.getClassName = function()
    {
        return perform("getClassName" , this.id , 0);
    };
    
    this.getParentElementId = function()
    {
        return perform("getParentElementId" , this.id , 0);
    };
    
    //! Return a list of the children's IDs. 
    this.getChildren = function()
    {
        return perform("getChildren" , this.id , 0 );
    };
    
    this.conformsTo = function( flags /* One or array*/ )
    {
        return perform("conformsTo" , this.id , flags )  ;
    };
    
    this.getChildrenConformingToType = function(flags )
    {
        return perform("getChildrenConformingToType" , this.id , flags )  ;
    };
    
    this.addObserverForAttribute = function( element , attribute)
    {
        perform("addObserverForAttribute" , this.id , [ element.id , attribute ]);
    };

}
