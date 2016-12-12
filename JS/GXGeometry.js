/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <manuel.deneu@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */

function GXPoint( x , y)
{
    this.x = x;
    this.y = y;
}

function GXSize( w , h)
{
    this.width  = w;
    this.height = h;
}

function GXRect( x , y , w , h)
{
    this.origin = new GXPoint( x , y );
    this.size   = new GXSize( w , h );
    
    this.isValid = function()
    {
        return ( (this.size.width != 0) && ( this.size.height != 0 ) )
    };
    
    /* **** */
    
    this.intersectsRect = function( otherRect )
    {
        return rectIntersectsRect( [this.origin.x      , this.origin.y      , this.size.width      , this.size.height      ] ,
                                   [otherRect.origin.x , otherRect.origin.y , otherRect.size.width , otherRect.size.height ]
                                  );
    };
    
    this.containsPoint = function( point )
    {
        return rectContainsPoint([this.origin.x      , this.origin.y      , this.size.width      , this.size.height      ] ,
                                 [ point.x , point.y ]
                                 );
    };
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

