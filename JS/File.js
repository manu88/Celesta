/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <manuel.deneu@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */



function _FileManager()
{
    this.getRessourceNamed = function( name )
    {
        return perform("getRessourceNamed" , getFileManager() , name );
    };

    this.getRessourceNamedWithExtension = function( name , extension )
    {
        return perform("getRessourceNamed" , getFileManager() , [name , extension] );
    };
    
    /* **** **** **** **** **** */

    this.createDirectory = function( pathName )
    {
        return perform("createDirectory" , getFileManager() , pathName );
    };
    
    this.deleteDirectory = function( pathName )
    {
        return perform("deleteDirectory" , getFileManager() , pathName );
    };

    /* **** **** **** **** **** */
    
    this.createFile = function( pathName )
    {
        return perform("createFile" , getFileManager() , pathName );
    };
    
    this.deleteFile = function( pathName )
    {
        return perform("deleteFile" , getFileManager() , pathName );
    };
}

var FileManager = new _FileManager();

/* **** **** **** **** **** **** **** **** */



function File( filePath )
{
    this.filePath = filePath;
    
    this.exists = function() //-> bool exists
    {
        return FILE.exists( this.filePath );
    };
    
    this.getSize = function() // -> int size ( -1 if file does not exist or error )
    {
        return FILE.getSize( this.filePath );
    };
    
    this.read = function()
    {
        return FILE.read( this.filePath );
    };
    
    this.write = function( buffer /*Text*/) //-> bool sucess
    {
        return FILE.write( this.filePath , "w"  , buffer );
    };
    
    this.append = function( buffer /*Text*/) //-> bool sucess
    {
        return FILE.write( this.filePath , "a"  , buffer );
    };
}





