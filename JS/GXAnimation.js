/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <manuel.deneu@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */



InitClass("GXAnimation");

/* All durations are in seconds! */

function GXAnimationMoveTo( point , duration)
{    
    return performStatic("GXAnimationMoveTo" , "GXAnimation" , [ point.x , point.y , duration ]);
}

function GXAnimationFadeTo( alpha , duration )
{
    return performStatic("GXAnimationFadeTo" , "GXAnimation" , [ alpha , duration ]);
}

function GXAnimationWait( duration)
{
    return performStatic("GXAnimationWait" , "GXAnimation" ,  duration );
}

function GXAnimationSequence(  array )
{
    return performStatic("GXAnimationSequence" , "GXAnimation" ,  array );
}

