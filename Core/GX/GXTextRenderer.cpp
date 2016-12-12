//
//  GXTextRenderer.cpp
//  Broadway_test
//
//  Created by Manuel Deneu on 10/10/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#include <assert.h>
#include "../Env_Variables.h"
#include "../Log/Log.h"
#include "../Config.h"
#include "GXTextRenderer.h"
#include "GXFont.h"
#include "GXPath.h"
#include "../GXDataType/Curve.h"

GXTextRenderer::GXTextRenderer():
_size      ( 10.f ),
_text      (  ""  ),
_font      ( nullptr ),
_textColor ( makeColor(255, 255, 255) ),
_textPath  ( nullptr ),
_textLengthInPix ( -1 )
{
    _sizeCoef = 1.0f/3000.f;
    
    _font = GXFont::getDefault();
    className = ClassNames::GXTextRenderer;
    setTransparency( true );
    
}

GXTextRenderer::~GXTextRenderer()
{

}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
 
    These setters cause a full redraw of the text, so setUnprepared() is called in order to perform
    the prepareRessources() method before drawing.
 
 */

void GXTextRenderer::setFont( const  GXFont* const font)
{
    if ( _font != font )
    {
        _font = font;
        
        setUnprepared();
    }
}

void GXTextRenderer::setDisplayedText( const std::string &text)
{
    if ( text != _text)
    {
        _text = text;
        
        setUnprepared();
    }
    
}

void GXTextRenderer::setSizeInPoints( float size)
{
    if ( _size != size )
    {
        setUnprepared();
        _size = size;
    }
}

void GXTextRenderer::setTextColor( const GXColor &color)
{
    _textColor = color;
    
    setUnprepared();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

// accessed by GUI thread only! 

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
void GXTextRenderer::changed()
{

    setUnprepared();
}

void GXTextRenderer::paint( const GXRect &rect , GXAnimation*  )
{
    printf("Absolute pos for %s is %i %i \n" , getElementName().c_str() , getAbsoluteCoords().x , getAbsoluteCoords().y);
    
    printf("Parent Element %i %i \n" ,getParentElement()->getBounds().origin.x ,getParentElement()->getBounds().origin.y );
    if (_font == nullptr)
        return;

    GXPath::beginTransformation();
    GXPath::translate(getAbsoluteCoords() );
    if (isOpaque())
        GXPath::clearRect(rect, getBackgroundColor() );

    GXPath::setFillColor( _textColor );
    GXPath::setStrokeColor( _textColor );
    _textPath->fill();
    _textPath->stroke();
    
    GXPath::endTransformation();


}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXTextRenderer::drawCharAtPositionWithSize( const GXGlyph *glyph , const GXPoint &pos , float size )
{

    std::function<int(const float&)> ceilAndCast = [] ( const float &val) ->int
    {
        return static_cast<int>( ceilf( val) );
    };
    

    
    for ( size_t i = 0 ; i < glyph->getNumSegments() ; i++)
    {
        const Curve_float *c = glyph->getCurveAt( i );
        
        if ( c->getCurveType() == CurveMoveTo )
        {
            const GXPoint p =  makePoint( ceilAndCast( c->getPointAt(0).getX() * size ),
                                          ceilAndCast( c->getPointAt(0).getY() * size )
                                         );

            _textPath->moveToPoint( pos + p );
        }
        
        else if ( c->getCurveType() == CurveLineTo )
        {
            const GXPoint p =  makePoint( ceilAndCast( c->getPointAt(0).getX() * size ),
                                          ceilAndCast( c->getPointAt(0).getY() * size )
                                         );
            _textPath->addLineToPoint( pos + p );
        }
        else if ( c->getCurveType() == CurveCubicTo )
        {
            const GXPoint p0 =  makePoint( ceilAndCast( c->getPointAt(0).getX() * size ),
                                          ceilAndCast( c->getPointAt(0).getY() * size )
                                         );
            
            const GXPoint c0 =  makePoint( ceilAndCast( c->getPointAt(1).getX() * size ),
                                          ceilAndCast( c->getPointAt(1).getY() * size )
                                         );
            
            const GXPoint p1 =  makePoint( ceilAndCast( c->getPointAt(2).getX() * size ),
                                           ceilAndCast( c->getPointAt(2).getY() * size )
                                         );
            
            _textPath->addCubicCurveToPoint( pos +   p0, pos +   c0, pos +   p1 );
        }
        
        else if ( c->getCurveType() == CurveQuadTo )
        {
            const GXPoint c0 =  makePoint( ceilAndCast( c->getPointAt(0).getX() * size ),
                                           ceilAndCast( c->getPointAt(0).getY() * size )
                                          );
            const GXPoint p0 =  makePoint( ceilAndCast( c->getPointAt(1).getX() * size ),
                                           ceilAndCast( c->getPointAt(1).getY() * size )
                                          );
            
            _textPath->addQuadCurveToPoint( pos + c0 , pos +p0 );
        }
        else
            DEBUG_ASSERT( false );
        
    }
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXTextRenderer::prepareRessources()
{
    const float realSize = static_cast<float>( _size* _sizeCoef );
    
    _textLengthInPix = -1;
    if ( _font == nullptr )
    {
        Log::log("\n Warning, no font set to display text ...");
        return ;
    }

    if ( _textPath == nullptr)
        _textPath = new GXPath( makeRectNULL() );

    _textPath->clear();
    
    const int lineJump =  static_cast<int>(_font->getLineSpace()* FUCKING_SCALE_CONV_ * realSize  );

    _textPath->setStrokeColor( _textColor );
    _textPath->setFillColor  ( _textColor );
    
    const GXPoint carret = makePoint(0,/* getBounds().origin.x, getBounds().origin.y*/
                                     0 - lineJump
                                     );
    
    GXPoint pt  = carret;
    GXSize size = makeSizeNULL();

    size.height = lineJump;
    
    int tempWidth = 0;
    
    

    for (unsigned i=0; i<_text.length(); ++i)
    {
        const char c =_text.at(i);
        
        if ( c == '\n')
        {
            pt.x = carret.x;
            pt.y -= lineJump;
            size.height += lineJump;
            
            if( tempWidth > size.width )
            {
                size.width = tempWidth;
                tempWidth = 0;
            }
        }
        
        else
        {
            const GXGlyph *glyph =  _font->getCharacter( c );

            drawCharAtPositionWithSize( glyph, pt , realSize );
            
            pt.x += (int ) glyph->getAdvanceX() * realSize;
            tempWidth += (int ) glyph->getAdvanceX() * realSize;

        }
    }
    
    _textLengthInPix = pt.x - carret.x;
    
    // last line
    if( tempWidth > size.width )
        size.width = tempWidth;
    
    setSize(makeSize( size.width, -size.height) );

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXTextRenderer::deleteRessources()
{
    if (_textPath != nullptr)
        delete _textPath;
    
    _textPath = nullptr;
}


#ifdef USE_JSON_PARSER
bool GXTextRenderer::addJSONinfos( JSON::Node &node) const
{
    if( getFont() != nullptr )
        node.addItem( GXElementAttributes::Font , Variant( getFont()->getName() ));
    
    node.addItem(GXElementAttributes::FontSize, Variant( getSizeInPoints() ));
    node.addItem(GXElementAttributes::Text , Variant( getDisplayedText() ));
    
    JSON::Node b = node.addArray( GXElementAttributes::TextColor );
    b.addItemToArray(Variant( getTextColor().r ) );
    b.addItemToArray(Variant( getTextColor().g ) );
    b.addItemToArray(Variant( getTextColor().b ) );
    b.addItemToArray(Variant( getTextColor().a ) );
    return  true;
}
#endif


#ifdef ENABLE_ELEMENT_SELECTOR
const Variant GXTextRenderer::performSelectorWithArguments( const std::string &selector , const VariantList  &arguments , bool *validCall)
{
    if( validCall)
        *validCall = true;
    
    if( selector =="setDisplayedText")
    {
        setDisplayedText( arguments.at(0).getString());
        return Variant::null();
    }
    else if (selector == "getDisplayedText")
    {
        return  Variant(getDisplayedText());
    }
    /*
    else if (selector == "setCarretPosition")
    {
        setCarretPosition( makePoint(arguments.at(0).getInt(), arguments.at(1).getInt()));
        return Variant::null();        
    }
    else if (selector == "getCarretPosition")
    {
        return Variant( { Variant(getCarretPosition().x ), Variant( getCarretPosition().y) } );
    }
     */
    else if( selector =="setSize")
    {
        setSizeInPoints( arguments.at(0).getFloat());
        return  Variant::null();
    }
    else if( selector == "getSize")
    {
        return  Variant(getSizeInPoints());
    }
    else if (selector == "setFont")
    {
        const std::string fontName = arguments.at(0).getString();
        
        const GXFont* font  = GXFont::getFontByName(fontName);
        
        if (font)
        {
            setFont( font );
            
            return  Variant( true );
        }
        return  Variant( false );
    }
    else if (selector == "getFont")
    {
        if( _font )
        {
            return Variant( getFont()->getName() );
        }
        
        return Variant::null();
    }
    else if ( selector == "setTextColor")
    {
        setTextColor( makeColor( static_cast<uint8_t>( arguments.at(0).getInt() ),
                                 static_cast<uint8_t>( arguments.at(1).getInt() ),
                                 static_cast<uint8_t>( arguments.at(2).getInt() ),
                                 static_cast<uint8_t>( arguments.at(3).getInt() ))
                     );
        
        return Variant::null();
    }
    else if (selector == "getTextColor")
    {
        return Variant( { Variant( getTextColor().r ) ,
                          Variant( getTextColor().g ) ,
                          Variant( getTextColor().b ) ,
                          Variant( getTextColor().a )
                      } );
    }
    
    if( validCall)
        *validCall = false;
    
    return GXElement::performSelectorWithArguments(selector, arguments, validCall);
}
#endif


