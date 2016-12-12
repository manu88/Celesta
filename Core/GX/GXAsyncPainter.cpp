//
//  GXAsyncPainter.cpp
//  Broadway_test
//
//  Created by Manuel Deneu on 08/10/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//
#include <assert.h>
#include "../Env_Variables.h"
#include "GXAsyncPainter.h"
#include "GXPath.h"

#include "../Config.h"
#include "DisplayController.h"





GXAsyncPainter::GXAsyncPainter():
_path ( nullptr )
{
    className = ClassNames::GXAsyncPainter;
}

GXAsyncPainter::~GXAsyncPainter()
{
    clear();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXAsyncPainter::setFillColor( const GXColor &fillColor)
{
    addPainter( new PainterFillColor( fillColor) );
}

void GXAsyncPainter::setStrokeColor( const GXColor &strokeColor)
{
    addPainter( new PainterStrokeColor( strokeColor) );
}

void GXAsyncPainter::setStrokeWidth( float width )
{
    addPainter( new PainterStrokeWidth( width ) );
}

void GXAsyncPainter::setCapStyle ( GXCapStyle style )
{
    addPainter( new PainterCapStyle( style ) );
}

void GXAsyncPainter::setJoinStyle( GXJoinStyle style )
{
    addPainter( new PainterJoinStyle( style ) );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXAsyncPainter::addRect( const GXRect &rect )
{
    addPainter( new PainterRect( rect ) );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXAsyncPainter::addRoundedRect( const GXRect &rect, float arcW , float arcH )
{
    addPainter( new PainterRoundedRect( rect , arcW , arcH ) );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXAsyncPainter::addEllipse( const GXRect &rect )
{
    addPainter( new PainterEllipse( rect ) );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXAsyncPainter::addArc( const GXPoint &center , const GXSize &size , float startAngle /* rads*/ , float angleExtent /* rads*/ ,GXArcType type )
{
    addPainter( new PainterArc( center , size , startAngle , angleExtent , type ) );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXAsyncPainter::addLine( const GXLine &line  )
{
    addPainter( new PainterLine( line ) );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */



void GXAsyncPainter::moveToPoint( const GXPoint &point)
{
    addPainter( new PainterMoveTo( point ) );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXAsyncPainter::addLineToPoint( const GXPoint &point)
{
    addPainter( new PainterLineTo( point ) );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXAsyncPainter::addCubicCurveToPoint( const GXPoint &controlPoint0 , const GXPoint &controlPoint1 , const GXPoint &endPoint )
{
    addPainter( new PainterCubicCurveTo( controlPoint0 , controlPoint1 , endPoint  ) );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXAsyncPainter::addQuadCurveToPoint( const GXPoint &controlPoint , const GXPoint &endPoint  )
{
    addPainter( new PainterQuadCurveTo( controlPoint , endPoint ) );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXAsyncPainter::close()
{
    addPainter( new PainterClose() );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXAsyncPainter::stroke()
{
    addPainter( new PainterStroke() );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXAsyncPainter::fill()
{
    addPainter( new PainterFill() );    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXAsyncPainter::addPainter( Painter *painter  )
{
        m_paintOperations.push_back( painter );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXAsyncPainter::clear()
{

    for( auto i : m_paintOperations )
    {
        if ( i )
        {
            delete ( i );
        }
    }
    m_paintOperations.clear();
    

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
    The following methods _MUST_ be called on GUI thread
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXAsyncPainter::changed()
{
    _path->setBounds( getBounds() );
}

void GXAsyncPainter::prepareRessources()
{
    _path = new GXPath( getBounds() );
}

void GXAsyncPainter::deleteRessources()
{
    if( _path)
    {
        delete _path;
        _path = nullptr;
    }

}



void GXAsyncPainter::paint( const GXRect &rect , GXAnimation* /*anim*/ )
{
    GXPath::beginTransformation();
    
    GXPath::translate( getAbsoluteCoords() );
    _path->setBounds( /*getBounds()*/rect );
    
    _path->shouldClearAfterRenderOps( true );

    for (auto i : m_paintOperations )
    {
        PainterType type = i->getType();
        
        switch ( type )
        {
            
            case PainterType_Rect:
            {
                const PainterRect* p = dynamic_cast<PainterRect*>( i );
                _path->addRect( p->getRect()  );
                
            }
                break;
            
            case PainterType_RoundedRect:
            {
                const PainterRoundedRect* p = dynamic_cast<PainterRoundedRect*>( i );
                _path->addRoundedRect( p->getRect()  , p->getArcW() , p->getArcH() );
                
            }
                break;
                
            case PainterType_Line:
            {
                const PainterLine* p = dynamic_cast<PainterLine*>( i );
                _path->addLine( p->getLine() );
                

            }
                break;
                
            case PainterType_Ellipse:
            {
                const PainterEllipse* p = dynamic_cast<PainterEllipse*>( i );
                _path->addEllipse( p->getRect() );
                

            }
                break;
                
            case PainterType_Arc:
            {
                const PainterArc* p = dynamic_cast<PainterArc*>( i );
                _path->addArc( p->getCenter() , p->getSize() , p->getStartAngle(), p->getAngleExtent(), p->getType() );
                
                
            }
                break;
                
            case PainterType_MoveTo:
            {
                const PainterMoveTo* p = dynamic_cast<PainterMoveTo*>( i );
                _path->moveToPoint( p->getPoint()  );
                

            }
                break;
                
            case PainterType_LineTo:
            {
                const PainterLineTo* p = dynamic_cast<PainterLineTo*>( i );
                _path->addLineToPoint( p->getPoint()  );
                

            }
                break;
                
            case PainterType_CubicCurveTo:
            {
                const PainterCubicCurveTo* p = dynamic_cast<PainterCubicCurveTo*>( i );
                _path->addCubicCurveToPoint( p->getCp0() ,
                                           p->getCp1() ,
                                           p->getEndPoint()
                                          );
                
                
            }
                break;
                
            case PainterType_QuadCurveTo:
            {
                const PainterQuadCurveTo* p = dynamic_cast<PainterQuadCurveTo*>( i );
                
                _path->addQuadCurveToPoint( p->getCp() ,
                                          p->getEndPoint()
                                         );
                
                break;
            }
                
            case PainterType_Close:
            {
                _path->closePath();

            }
                break;
                
            case PainterType_ColorFill:
            {
                PainterFillColor* p = dynamic_cast<PainterFillColor*>( i );
                _path->setFillColor( p->m_color);


            }
                break;
                
            case PainterType_ColorStroke:
            {
                PainterStrokeColor* p = dynamic_cast<PainterStrokeColor*>( i );
                _path->setStrokeColor( p->m_color);


            }
                break;
                
            case PainterType_StrokeWidth:
            {
                PainterStrokeWidth* p = dynamic_cast< PainterStrokeWidth* >( i );
                _path->setStrokeWidth( p->getWidth() );
                
                
            }
                break;
                
            case PainterType_CapStyle:
            {
                PainterCapStyle* p = dynamic_cast< PainterCapStyle* >( i );
                _path->setCapStyle( p->style );
                
                
            }
                break;
                
            case PainterType_JoinStyle:
            {
                PainterJoinStyle* p = dynamic_cast< PainterJoinStyle* >( i );
                _path->setJoinStyle( p->style );
                
                
            }
                break;
                
            case PainterType_Fill:

                _path->fill();


                break;
                
            case PainterType_Stroke:

                _path->stroke();

                break;

            default:
                /* Each case should have been covered ... */
                DEBUG_ASSERT( false );
                break;
        }

    }
    
    GXPath::endTransformation();

    
}

#ifdef USE_JSON_PARSER
bool GXAsyncPainter::addJSONinfos( JSON::Node &node) const
{
    JSON::Node painter = node.addArray("painter");

    for (auto i : m_paintOperations )
    {
        PainterType type = i->getType();
        
        switch ( type )
        {
            
            case PainterType_Rect:
            {
                const PainterRect* p = dynamic_cast<PainterRect*>( i );
                
                JSON::Node r;
                JSON::Node rect = r.addArray( PainterTypeAttribue::Rect );
                
                rect.addItemToArray( Variant(p->getRect().origin.x ));
                rect.addItemToArray( Variant(p->getRect().origin.y ));
                rect.addItemToArray( Variant(p->getRect().size.width ));
                rect.addItemToArray( Variant(p->getRect().size.height ));
                
                painter.addItemToArray(r);
            }
            break;
            
            case PainterType_RoundedRect:
            {
                const PainterRoundedRect* p = dynamic_cast<PainterRoundedRect*>( i );
                
                JSON::Node r;
                JSON::Node rect = r.addArray(PainterTypeAttribue::RoundedRect );
                
                rect.addItemToArray( Variant(p->getRect().origin.x ));
                rect.addItemToArray( Variant(p->getRect().origin.y ));
                rect.addItemToArray( Variant(p->getRect().size.width ));
                rect.addItemToArray( Variant(p->getRect().size.height ));
                rect.addItemToArray( Variant(p->getArcW() ));
                rect.addItemToArray( Variant(p->getArcH() ));
                
                painter.addItemToArray(r);
                
            }
            break;
            
            case PainterType_Line:
            {
                const PainterLine* p = dynamic_cast<PainterLine*>( i );
                
                JSON::Node r;
                JSON::Node rect = r.addArray(PainterTypeAttribue::Line);
                
                rect.addItemToArray( Variant(p->getLine().p0.x ));
                rect.addItemToArray( Variant(p->getLine().p0.y ));
                rect.addItemToArray( Variant(p->getLine().p1.x ));
                rect.addItemToArray( Variant(p->getLine().p1.y ));
                
                painter.addItemToArray(r);

            }
            break;
            
            case PainterType_Ellipse:
            {
                const PainterEllipse* p = dynamic_cast<PainterEllipse*>( i );
                
                JSON::Node r;
                JSON::Node rect = r.addArray( PainterTypeAttribue::Ellipse );
                
                rect.addItemToArray( Variant(p->getRect().origin.x ));
                rect.addItemToArray( Variant(p->getRect().origin.y ));
                rect.addItemToArray( Variant(p->getRect().size.width ));
                rect.addItemToArray( Variant(p->getRect().size.height ));
                
                painter.addItemToArray(r);
                
                
                
            }
            break;
            
            case PainterType_Arc:
            {
                const PainterArc* p = dynamic_cast<PainterArc*>( i );
                
                JSON::Node r;
                JSON::Node rect = r.addArray( PainterTypeAttribue::Arc );
                
                rect.addItemToArray( Variant( p->getCenter().x ));
                rect.addItemToArray( Variant( p->getCenter().y ));
                rect.addItemToArray( Variant( p->getSize().width ));
                rect.addItemToArray( Variant( p->getSize().height ));
                rect.addItemToArray( Variant( p->getStartAngle() ));
                rect.addItemToArray( Variant( p->getAngleExtent() ));
                rect.addItemToArray( Variant( p->getType() ));
                
                painter.addItemToArray(r);
                
                
            }
            break;
            
            case PainterType_MoveTo:
            {
                const PainterMoveTo* p = dynamic_cast<PainterMoveTo*>( i );
                JSON::Node r;
                JSON::Node rect = r.addArray( PainterTypeAttribue::MoveTo );
                
                rect.addItemToArray( Variant( p->getPoint().x ));
                rect.addItemToArray( Variant( p->getPoint().y ));
                
                painter.addItemToArray(r);
                
                
            }
            break;
            
            case PainterType_LineTo:
            {
                const PainterLineTo* p = dynamic_cast<PainterLineTo*>( i );
                JSON::Node r;
                JSON::Node rect = r.addArray( PainterTypeAttribue::LineTo );
                
                rect.addItemToArray( Variant( p->getPoint().x ));
                rect.addItemToArray( Variant( p->getPoint().y ));
                
                painter.addItemToArray(r);
                
                
            }
            break;
            
            case PainterType_CubicCurveTo:
            {
                const PainterCubicCurveTo* p = dynamic_cast<PainterCubicCurveTo*>( i );

                JSON::Node r;
                JSON::Node rect = r.addArray( PainterTypeAttribue::CubicCurveTo );
                
                rect.addItemToArray( Variant( p->getCp0().x ));
                rect.addItemToArray( Variant( p->getCp0().y ));
                rect.addItemToArray( Variant( p->getCp1().x ));
                rect.addItemToArray( Variant( p->getCp1().y ));
                rect.addItemToArray( Variant( p->getEndPoint().x ));
                rect.addItemToArray( Variant( p->getEndPoint().y ));
                
                painter.addItemToArray(r);
                
            }
            break;
            
            case PainterType_QuadCurveTo:
            {
                const PainterQuadCurveTo* p = dynamic_cast<PainterQuadCurveTo*>( i );

                JSON::Node r;
                JSON::Node rect = r.addArray( PainterTypeAttribue::QuadCurveTo );
                
                rect.addItemToArray( Variant( p->getCp().x ));
                rect.addItemToArray( Variant( p->getCp().y ));
                rect.addItemToArray( Variant( p->getEndPoint().x ));
                rect.addItemToArray( Variant( p->getEndPoint().y ));
                
                painter.addItemToArray(r);

            }
            break;
            case PainterType_Close:
            {
                JSON::Node r;
                JSON::Node rect = r.addArray( PainterTypeAttribue::Close );
                
                rect.addItemToArray( Variant( 0 ));

                painter.addItemToArray(r);
            }
            break;
            
            case PainterType_ColorFill:
            {
                PainterFillColor* p = dynamic_cast<PainterFillColor*>( i );
                
                JSON::Node r;
                JSON::Node rect = r.addArray(PainterTypeAttribue::ColorFill);
                
                rect.addItemToArray( Variant(p->m_color.r) );
                rect.addItemToArray( Variant(p->m_color.g) );
                rect.addItemToArray( Variant(p->m_color.b) );
                rect.addItemToArray( Variant(p->m_color.a) );
                
                painter.addItemToArray(r);
                
            }
            break;
            
            case PainterType_ColorStroke:
            {
                PainterStrokeColor* p = dynamic_cast<PainterStrokeColor*>( i );

                
                JSON::Node r;
                JSON::Node rect = r.addArray(PainterTypeAttribue::ColorStroke);
                
                rect.addItemToArray( Variant(p->m_color.r) );
                rect.addItemToArray( Variant(p->m_color.g) );
                rect.addItemToArray( Variant(p->m_color.b) );
                rect.addItemToArray( Variant(p->m_color.a) );

                painter.addItemToArray(r);
                
                
            }
            break;
            
            case PainterType_StrokeWidth:
            {
                PainterStrokeWidth* p = dynamic_cast< PainterStrokeWidth* >( i );

                JSON::Node r;
                JSON::Node rect = r.addArray(PainterTypeAttribue::StrokeWidth);
                
                rect.addItemToArray( Variant(p->getWidth() ) );
                painter.addItemToArray(r);
                
                
            }
            break;
            
            case PainterType_CapStyle:
            {
                PainterCapStyle* p = dynamic_cast< PainterCapStyle* >( i );

                JSON::Node r;
                JSON::Node rect = r.addArray(PainterTypeAttribue::CapStyle);
                
                rect.addItemToArray( Variant(p->style ) );
                painter.addItemToArray(r);
                
                
            }
            break;
            
            case PainterType_JoinStyle:
            {
                PainterJoinStyle* p = dynamic_cast< PainterJoinStyle* >( i );
                JSON::Node r;
                JSON::Node rect = r.addArray(PainterTypeAttribue::JoinStyle);
                
                rect.addItemToArray( Variant(p->style ) );
                painter.addItemToArray(r);

                
                
            }
            break;
            
            case PainterType_Fill:
            {
                JSON::Node r;
                JSON::Node rect = r.addArray(PainterTypeAttribue::Fill);
                
                rect.addItemToArray( Variant(0 ) );
                painter.addItemToArray(r);
            }
            
            break;
            
            case PainterType_Stroke:
            {
                JSON::Node r;
                JSON::Node rect = r.addArray(PainterTypeAttribue::Stroke);
                
                rect.addItemToArray( Variant(0 ) );
                painter.addItemToArray(r);
            }
            break;
            
            default:

            break;
        }
        
    }
    
    return  true;
}
#endif

#ifdef ENABLE_ELEMENT_SELECTOR
const Variant GXAsyncPainter::performSelectorWithArguments( const std::string &selector , const VariantList  &arguments , bool *validCall)
{
    if (validCall)
        *validCall = true;
    
    
    if (selector == "setFillColor")
    {
        setFillColor( makeColor( arguments.at(0).getInt(),
                                 arguments.at(1).getInt(),
                                 arguments.at(2).getInt(),
                                 arguments.at(3).getInt()
                                )
                     );
        
        return Variant::null();
    }
    else if (selector == "setStrokeColor")
    {
        setStrokeColor( makeColor( arguments.at(0).getInt(),
                                   arguments.at(1).getInt(),
                                   arguments.at(2).getInt(),
                                   arguments.at(3).getInt()
                                )
                     );
        
        return Variant::null();
    }
    else if ( selector == "setStrokeWidth")
    {
        setStrokeWidth( arguments.at(0).getFloat() );
        return Variant::null();
    }
    
    else if (selector == "addRect")
    {
        addRect( makeRect(arguments.at(0).getInt(),
                          arguments.at(1).getInt(),
                          arguments.at(2).getInt(),
                          arguments.at(3).getInt()
                          )
                );
        return Variant::null();
    }
    
    else if (selector == "addRoundedRect")
    {
        addRoundedRect( makeRect(arguments.at(0).getInt(),
                                 arguments.at(1).getInt(),
                                 arguments.at(2).getInt(),
                                 arguments.at(3).getInt()
                                 ),
                                 
                          arguments.at(4).getFloat(),
                          arguments.at(5).getFloat()
                );
        return Variant::null();
    }
    else if (selector == "addEllipse")
    {
        addEllipse( makeRect(arguments.at(0).getInt(),
                             arguments.at(1).getInt(),
                             arguments.at(2).getInt(),
                             arguments.at(3).getInt()
                            )
                );
        return Variant::null();
    }
    else if (selector == "addLine")
    {
        addLine( makeLine(arguments.at(0).getInt(),
                          arguments.at(1).getInt(),
                          arguments.at(2).getInt(),
                          arguments.at(3).getInt()
                          )
                );
        return Variant::null();
    }
    else if (selector == "stroke")
    {
        stroke();
        return Variant::null();
    }
    else if (selector == "fill")
    {
        fill();
        return Variant::null();
    }
    else if (selector == "clear")
    {
        clear();
        return Variant::null();
    }
    /*
    void moveToPoint         ( const GXPoint &point);
    void addLineToPoint      ( const GXPoint &point);
    void addCubicCurveToPoint( const GXPoint &controlPoint0 , const GXPoint &controlPoint1 , const GXPoint &endPoint );
    void addQuadCurveToPoint ( const GXPoint &controlPoint , const GXPoint &endPoint  );
    void close();
    

    void fill();
    
    void clear();
     
     void setCapStyle ( GXCapStyle style );
     void setJoinStyle( GXJoinStyle style );
     void addRoundedRect( const GXRect &rect, float arcW , float arcH );
     void addArc( const GXPoint &center ,  const GXSize &size , float startAngle , float angleExtent,GXArcType type );
     */

    if (validCall)
        *validCall = false;
    
    return GXElement::performSelectorWithArguments(selector, arguments, validCall);
}
#endif
