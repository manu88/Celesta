//
//  GXFont.h
//  Broadway_test
//
//  Created by Manuel Deneu on 10/10/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_test__GXFont__
#define __Broadway_test__GXFont__

#include <unordered_set>
#include <vector>
#include <algorithm>
#include "../Env_Variables.h"
#include "../Internal/Object.h"
#include "../GXDataType/Curve.h"

#include "../Log/Log.h"



class FontParser;

/* **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** */

class GXGlyph : public Object
{
    friend class FontParser;    
public:
    GXGlyph();
    ~GXGlyph();
    
    void addSegment( Curve_float *curve);
    
    size_t getNumSegments() const noexcept
    {
        return _segments.size();
    }
    
    const Curve_float* getCurveAt( size_t index ) const
    {
        return _segments.at( index );
    }
    
    
    float getAdvanceX() const
    {
        return _advanceX;
    }
    
    float getAdvanceY() const
    {
        return _advanceY;
    }
    
private:
    float _advanceX;
    float _advanceY;
    
    std::vector< Curve_float*> _segments;
    

};

/* **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** */

class GXFont : public Object
{
    friend class FontParser;
    
public:
    
    // screen resolution in DPI : size in pix = size in points * resolution in DPI / 72
    enum { DPI_Res = 72 };

    static const GXFont* const loadFont( const std::string &fileName);
    
    static const GXFont* const getFontByName    ( const std::string &fontName);
    static const GXFont* const getFontByFileName( const std::string &fileName);
    
    static const GXFont* const getDefault()
    {
        return getFontByName( DefaultsValues::DefaultFont );
    }
    
    static size_t getNumFontsLoaded() 
    {
        return s_fontsAtlas.size();
    }

    static const std::unordered_set<GXFont*> &getFontAtlas()
    {
        return s_fontsAtlas;
    }
    
    static void deleteLoadedFonts();
    
    /* **** **** **** **** **** **** */
    
    int getLineSpace() const
    {
        return m_lineSpace;
    }
    
    std::string getName() const
    {
        return m_fontName;
    }
    
    std::string getFileName() const
    {
        return m_fileName;
    }
    
    unsigned long getNumGlyphes() const
    {
        return m_glyphes.size();
    }

    const GXGlyph* getCharacter(const char c) const
    {
        auto vectorIt = std::find_if(m_glyphes.begin(),m_glyphes.end(), CompareFirst( c ) );
        
        if ( vectorIt != m_glyphes.end() )
            return vectorIt->second;
        
        return nullptr;
    }
    

protected:
    
    void addChar( char c , GXGlyph *glyphe );
    
    // Instances are created by FontParser
    GXFont();
    ~GXFont();
    
private:
    
    void changeGlyphesSize();
    
    // font settings
    
    std::string m_fontName;
    int         m_lineSpace;
    std::string m_fileName;

    std::vector< std::pair<char , GXGlyph* > > m_glyphes;
    
    /* *** *** */
    
    // predicate for finding glyph by char
    struct CompareFirst
    {
        CompareFirst( char c) : _char(c) {}
        
        bool operator()(const std::pair<char,GXGlyph* >& elem) const
        {
            return _char == elem.first;
        }
        
    private:
        char _char;
    };
    
    // font atlas
    static std::unordered_set<GXFont*> s_fontsAtlas;
};

#endif /* defined(__Broadway_test__GXFont__) */
