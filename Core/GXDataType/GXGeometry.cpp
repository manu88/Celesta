//
//  GXGeometry.cpp
//  
//
//  Created by Manuel Deneu on 28/08/14.
//
//

#include <stdio.h>
#include "GXGeometry.h"

GXSize makeSizeNULL()
{
    return GXSize { 0 , 0 };
}

GXSize makeSize( int width , int height )
{
    return GXSize { width , height };
}

/* **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** */
/*! \fn makePoint( int x , int y )
 \brief Creates a point with {x,y} coordinates
 \param x x coordinate
 \param y y coordinate
 \return A point at {x,y}.
 */
GXPoint makePoint( int x , int y )
{
    return GXPoint {  x ,  y};
}

/* **** **** **** **** **** **** **** **** **** **** **** */
/*! \fn makePointNULL()
 \brief Creates a point with {0,0} coordinates
 \return A point at {0,0}.
 */
GXPoint makePointNULL()
{
    return GXPoint {  0 ,  0};
}

/* **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** */
/*! \fn makeLine( GXPoint p0 , GXLine p1)
 \brief Creates a line crossing p0 and p1.
 \param p0 A point.
 \param p1 An other point.
 \return A Line.
 */
GXLine makeLine( const GXPoint &p0 , const GXPoint &p1 )
{
    return GXLine { p0 ,  p1 };
}

GXLine makeLine( int p0x , int p0y , int p1x , int p1y)
{
    return GXLine { makePoint(p0x, p0y) ,  makePoint(p1x, p1y) };
}

/* **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** */
/*! \fn makeRect( GXPoint origin , GXPoint size )
 \brief Creates a rectangle with an origin (top-left corner) and a size.
 \param origin Top-left Corner
 \param size   GXPoint structure containing the width and height as coordinates
 \return A Rectangle.
 */
GXRect makeRect( const GXPoint &origin , const GXSize &size )
{
    return GXRect { origin ,  size};
}

/* **** **** **** **** **** **** **** **** **** **** **** */
/*! \fn makeRect( int x , int y , int width , int height )
 \brief Creates a rectangle with an origin (top-left corner) and a size.
 \param x Top-left Corner's x coordinate
 \param y Top-left Corner's x coordinate
 \param width rectangle's width
 \param height rectangle's height
 \return A Rectangle.
 */
GXRect makeRect( int x , int y , int width , int height )
{
    return GXRect {  makePoint( x , y) , makeSize(width , height ) };
}

/* **** **** **** **** **** **** **** **** **** **** **** */
/*! \fn makeRectNULL()
 \brief Creates a rectangle with {0,0} coordinates and size
 \return A null rectangle.
 */
GXRect makeRectNULL()
{
    return GXRect {  makePointNULL() , makeSizeNULL() };
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/
/*
    Geometry tests
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

bool rectContainsPoint( const GXRect &rect ,const GXPoint &pt )
{
    return (    ( pt.x >= rect.origin.x  ) && ( pt.x <= rect.origin.x + rect.size.width  )
             && ( pt.y >= rect.origin.y  ) && ( pt.y <= rect.origin.y + rect.size.height  )
           );
}

bool rectIntersectsRect(const GXRect &rect1 ,const GXRect &rect2 )
{
    int l1 = rect1.origin.x;
    int r1 = l1;
    if (rect1.size.width < 0)
        l1 += rect1.size.width;
    else
        r1 += rect1.size.width;
    
    if (l1 == r1) // null rect
        return false;
    
    int l2 = rect2.origin.x;
    int r2 = l2;
    if (rect2.size.width < 0)
        l2 += rect2.size.width;
    else
        r2 += rect2.size.width;
    
    if (l2 == r2) // null rect
        return false;
    
    if (l1 >= r2 || l2 >= r1)
        return false;
    
    int t1 = rect1.origin.y;
    int b1 = t1;
    if ( rect1.size.height < 0)
        t1 += rect1.size.height;
    else
        b1 += rect1.size.height;
    
    if (t1 == b1) // null rect
        return false;
    
    int t2 = rect2.origin.y;
    int b2 = t2;
    if (rect2.size.height < 0)
        t2 += rect2.size.height;
    else
        b2 += rect2.size.height;
    
    if (t2 == b2) // null rect
        return false;
    
    if (t1 >= b2 || t2 >= b1)
        return false;
    
    return true;
}

GXRect rectIntersection( const GXRect &rect1 ,const GXRect &rect2 )
{/*
    if (!rectIntersectsRect(rect1, rect2))
        return makeRectNULL();
   */ 
    
/**/
    int l1 = rect1.origin.x;
    int r1 = rect1.origin.x;
    if (rect1.size.width < 0)
        l1 += rect1.size.width;
    else
        r1 += rect1.size.width;
    
    if (l1 == r1) // null rect
        return makeRectNULL();
    
    int l2 = rect2.origin.x;
    int r2 = rect2.origin.x;
    if (rect2.size.width < 0)
        l2 += rect2.size.width;
    else
        r2 += rect2.size.width;
    
    if (l2 == r2) // null rect
        return makeRectNULL();
    
    if (l1 >= r2 || l2 >= r1)
        return makeRectNULL();
    
    int t1 = rect1.origin.y;
    int b1 = rect1.origin.y;
    if (rect1.size.height < 0)
        t1 += rect1.size.height;
    else
        b1 += rect1.size.height;
    if (t1 == b1) // null rect
        return makeRectNULL();
    
    int t2 = rect2.origin.y;
    int b2 = rect2.origin.y;
    if (rect2.size.height < 0)
        t2 += rect2.size.height;
    else
        b2 += rect2.size.height;
    
    if (t2 == b2) // null rect
        return makeRectNULL();
    
    if (t1 >= b2 || t2 >= b1)
        return makeRectNULL();
    
    GXRect tmp;
    tmp.origin.x = std::max(l1, l2);
    tmp.origin.y = std::max(t1, t2);
    tmp.size.width = std::min(r1, r2) - tmp.origin.x;
    tmp.size.height = std::min(b1, b2) - tmp.origin.y;
    return tmp;
    /**/
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/
/*
 Operators
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

bool operator==(const GXPoint& lhs, const GXPoint& rhs)
{
    return (    ( lhs.x == rhs.x )
            &&  ( lhs.y == rhs.y )
            );
}

bool operator==(const GXSize& lhs, const GXSize& rhs)
{
    return (    ( lhs.width == rhs.width )
            &&  ( lhs.height == rhs.height )
            );
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

bool operator==(const GXPoint& lhs, const GXSize& rhs)
{
    return (    ( lhs.x == rhs.width )
            &&  ( lhs.y == rhs.height )
            );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

bool operator!=(const GXPoint& lhs, const GXPoint& rhs)
{
    return !( lhs == rhs);
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

bool operator!=(const GXSize&  lhs, const GXSize&  rhs)
{
    return !( lhs == rhs);
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

GXPoint operator+(const GXPoint& lhs, const GXPoint& rhs)
{
    return GXPoint {  lhs.x + rhs.x ,  lhs.y + rhs.y };
}

GXPoint operator+(const GXPoint& lhs, const GXSize& rhs)
{
    return GXPoint {  lhs.x + rhs.width ,  lhs.y + rhs.height };
}

GXPoint operator+(const GXSize& lhs, const GXPoint& rhs)
{
    return GXPoint {  lhs.width + rhs.x ,  lhs.height + rhs.y };
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

GXPoint operator-(const GXPoint& lhs, const GXPoint& rhs)
{
    return GXPoint {  lhs.x - rhs.x ,  lhs.y - rhs.y };
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

GXPoint operator+=(const GXPoint& lhs, const GXPoint& rhs)
{
    return GXPoint {  lhs.x + rhs.x ,  lhs.y + rhs.y };
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

GXPoint operator-=(const GXPoint& lhs, const GXPoint& rhs)
{
    return GXPoint {  lhs.x - rhs.x ,  lhs.y - rhs.y };    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

bool operator==(const GXRect& lhs, const GXRect& rhs)
{
    return (    ( lhs.origin == rhs.origin )
            &&  ( lhs.size   == rhs.size )
            );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

bool operator!=(const GXRect& lhs, const GXRect& rhs)
{
    return !( lhs == rhs);
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

GXRect operator+=(const GXRect& lhs, const GXPoint& rhs)
{
    return  makeRect(lhs.origin.x    + rhs.x,
                     lhs.origin.y    + rhs.y,
                     lhs.size.width  ,
                     lhs.size.height );// GXRect { lhs.origin + rhs , lhs.size + rhs };
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

GXRect operator-=(const GXRect& lhs, const GXPoint& rhs)
{
//    return GXRect { lhs.origin - rhs , lhs.size - rhs };
    return  makeRect(lhs.origin.x    - rhs.x,
                     lhs.origin.y    - rhs.y,
                     lhs.size.width  ,
                     lhs.size.height );
}

GXRect operator+(const GXRect& lhs, const GXPoint& rhs)
{
    return  makeRect(lhs.origin.x    + rhs.x,
                     lhs.origin.y    + rhs.y,
                     lhs.size.width  ,
                     lhs.size.height );// GXRect { lhs.origin + rhs , lhs.size + rhs };
}
GXRect operator-(const GXRect& lhs, const GXPoint& rhs)
{
    return  makeRect(lhs.origin.x    - rhs.x,
                     lhs.origin.y    - rhs.y,
                     lhs.size.width ,
                     lhs.size.height);
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

GXLine operator+=(const GXLine& lhs, const GXPoint& rhs)
{
    return makeLine(lhs.p0 + rhs, lhs.p1 + rhs);
}

GXLine operator-=(const GXLine& lhs, const GXPoint& rhs)
{
    return makeLine(lhs.p0 - rhs, lhs.p1 - rhs);
}

GXLine operator+(const GXLine& lhs, const GXPoint& rhs)
{
    return makeLine(lhs.p0 + rhs, lhs.p1 + rhs);
}

GXLine operator-(const GXLine& lhs, const GXPoint& rhs)
{
    return makeLine(lhs.p0 - rhs, lhs.p1 - rhs);
}



