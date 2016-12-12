//
//  GXFont.cpp
//  Broadway_test
//
//  Created by Manuel Deneu on 10/10/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//
#include "../Env_Variables.h"
#include "GXFont.h"
#include "../Parsers/FontParser.h"





GXGlyph::GXGlyph()
{
    className = ClassNames::GXGlyph;
}

GXGlyph::~GXGlyph()
{
    for (auto i : _segments )
    {
        delete i;
    }
    _segments.clear();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXGlyph::addSegment( Curve_float *curve)
{
    _segments.push_back( curve );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

std::unordered_set<GXFont*> GXFont::s_fontsAtlas;

GXFont::GXFont() :
m_fontName      ( "" ),
m_lineSpace     ( 0  ),
m_fileName      ( "" )
{
    className = ClassNames::GXFont;
    
}

GXFont::~GXFont()
{
    for (auto i : m_glyphes)
        delete i.second;
    
    m_glyphes.clear();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

const GXFont* const GXFont::loadFont( const std::string &fileName)
{
    FontParser p;
    
    const GXFont* font = getFontByName( p.getFontName( fileName ) );
    
    if( (font != nullptr ))
        return font;

    GXFont *f = new GXFont();
        
    if ( p.parseFontFile( f , fileName ) )
        s_fontsAtlas.insert( f );
    else
    {
        delete f;
        f = nullptr;
    }

    return f;
}



/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ const GXFont* const  GXFont::getFontByName( const std::string &fontName)
{
    if ( fontName == DefaultsValues::Undefined)
        return nullptr;
    
    for ( const GXFont *font :s_fontsAtlas )
    {
        if ( font->m_fontName == fontName )
            return font;
    }
    return nullptr;
}
/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ const GXFont* const GXFont::getFontByFileName( const std::string &fileName)
{
    for (  const GXFont *font :  s_fontsAtlas )
    {
        if ( font->m_fileName == fileName )
            return font;
    }
    
    return nullptr;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ void GXFont::deleteLoadedFonts()
{
    for ( auto i : s_fontsAtlas )
    {

        delete i;
    }
    
    s_fontsAtlas.clear();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXFont::addChar(char c , GXGlyph *glyphe)
{
    m_glyphes.push_back( std::make_pair( c , glyphe ) );
    
}

void GXFont::changeGlyphesSize()
{
    
}

