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
import("UI/UIBase.js");
import("UI/UIAlert.js");
import("UI/UISlider.js");

function UI()
{
    this.prototype = new GXScene();
    this.root = 0;

    this.parseJSON = function ( file )
    {
        if( !FILE.exists(file ))
            return false;
        
        root = JSON.parse( FILE.read(file ) );
        
        
        this.setElementName(root.title );
        print(root.title + "\n");
        
        var folder = root.folder;
        
        print(" num GXElements = " +root.children.length  + "\n");
        
        for ( var i = 0; i < root.children.length ; i++)
        {
            var child  = root.children[i];
            print("Name = "+child.name + "\n");
            print("Type = "+child.type + "\n");
            print("Bounds = " + child.bounds[0]  + " "+ child.bounds[1]  + " " + child.bounds[2]  + " "+ child.bounds[3]  + "\n");
            var chd;

            
            if( child.type == "GXImage")
            {
                chd = new GXImage();
                chd.setFileSource( child.ressource );
            }

            else if( child.type == "GXPath")
            {
                chd = new GXPath();
                chd.addRect( 100, 1 , 402 , 900);
                chd.setStrokeColor( 1 ,2,3,4);
                chd.addRoundedRect( 80, 10 , 402 , 1000 , 3 ,4);
                
            }
            else if( child.type == "GXText")
            {
                chd = new GXText();
                chd.setText( child.text );
                chd.setFont( child.font );
                chd.setSize( child.fontSize );
            }
            chd.setElementName( child.name );
            chd.setLayer( child.layer );
            chd.setBounds(child.bounds[0] , child.bounds[1] , child.bounds[2] , child.bounds[3] );

            this.addElement( chd );
        }
        
        
        LogElementTree();
        return true;
    };
}



